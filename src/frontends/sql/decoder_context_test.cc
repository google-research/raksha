//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/frontends/sql/decoder_context.h"

#include "src/common/testing/gtest.h"
#include "src/frontends/sql/testing/literal_operation_view.h"
#include "src/frontends/sql/testing/merge_operation_view.h"
#include "src/frontends/sql/testing/tag_transform_operation_view.h"

namespace raksha::frontends::sql {

using ::testing::Combine;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::ResultOf;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::Values;
using ::testing::ValuesIn;

using ir::Attribute;
using ir::Block;
using ir::IRContext;
using ir::NamedValueMap;
using ir::Operation;
using ir::Operator;
using ir::Storage;
using ir::Value;
using ir::types::TypeBase;
using ir::value::Any;
using ir::value::BlockArgument;
using ir::value::OperationResult;
using ir::value::StoredValue;

TEST(DecoderContextTest, BuildTopLevelBlock) {
  IRContext context;
  DecoderContext decoder_context(context);

  const Block &block = decoder_context.BuildTopLevelBlock();
  EXPECT_EQ(block.parent_module(), &decoder_context.global_module());
}

TEST(DecoderContextTest, GetOrCreateStorageIdempotence) {
  IRContext context;
  DecoderContext decoder_context(context);
  const Storage &store1 = decoder_context.GetOrCreateStorage("Table1");
  const Storage &store2 = decoder_context.GetOrCreateStorage("Disk1");
  const Storage &store1_again = decoder_context.GetOrCreateStorage("Table1");
  const Storage &store2_again = decoder_context.GetOrCreateStorage("Disk1");

  EXPECT_EQ(&store1, &store1_again);
  EXPECT_EQ(&store2, &store2_again);
  EXPECT_NE(&store1, &store2);

  EXPECT_EQ(store1.type().type_base().kind(), TypeBase::Kind::kPrimitive);
  EXPECT_EQ(store2.type().type_base().kind(), TypeBase::Kind::kPrimitive);
}

class LiteralOpTest : public TestWithParam<absl::string_view> {};

TEST_P(LiteralOpTest, MakeLiteralOperationTest) {
  IRContext context;
  DecoderContext decoder_context(context);

  absl::string_view literal_str = GetParam();
  const Operation &lit_op = decoder_context.MakeLiteralOperation(literal_str);

  const Block &top_level_block = decoder_context.BuildTopLevelBlock();

  EXPECT_THAT(lit_op.parent(), &top_level_block);
  EXPECT_THAT(lit_op.impl_module(), IsNull());
  testing::LiteralOperationView literal_operation_view(lit_op);
  EXPECT_EQ(literal_operation_view.GetLiteralStr(), literal_str);
}

INSTANTIATE_TEST_SUITE_P(LiteralOpTest, LiteralOpTest,
                         Values("val1", "1000", "3.1415", "true", ""));

TEST(DecoderContextTest, DecoderContextRegisterAndGetValueTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  Storage storage("storage1", ir_context.type_factory().MakePrimitiveType());

  Value value1{Any()};
  decoder_context.RegisterValue(1, value1);
  EXPECT_THAT(value1.If<Any>(), NotNull());
  Value value2{StoredValue(storage)};
  decoder_context.RegisterValue(2, value2);
  Value get_value1 = decoder_context.GetValue(1);
  Value get_value2 = decoder_context.GetValue(2);
  EXPECT_EQ(get_value1, value1);
  EXPECT_EQ(get_value2, value2);
}

TEST(DecoderContextDeathTest, DecoderContextValueRegistryDeathTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  decoder_context.RegisterValue(1, Value(Any()));
  decoder_context.RegisterValue(2, Value(Any()));

  EXPECT_DEATH({ decoder_context.RegisterValue(1, Value(Any())); },
               "id_to_value map has more than one value associated with the id "
               "1.");
  EXPECT_DEATH({ decoder_context.RegisterValue(2, Value(Any())); },
               "id_to_value map has more than one value associated with the id "
               "2.");
  EXPECT_DEATH({ decoder_context.GetValue(3); },
               "Could not find a value with id 3.");
}

class DecodeMergeOpTest
    : public TestWithParam<std::tuple<std::vector<Value>, std::vector<Value>>> {
};

TEST_P(DecodeMergeOpTest, DecodeMergeOpTest) {
  auto &[direct_inputs, control_inputs] = GetParam();
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  const Operation &op =
      decoder_context.MakeMergeOperation(direct_inputs, control_inputs);
  const Block &top_block = decoder_context.BuildTopLevelBlock();

  EXPECT_EQ(op.parent(), &top_block);

  testing::MergeOperationView merge_op_view(op);

  // Test the vectors for equivalence.
  EXPECT_EQ(merge_op_view.GetDirectInputs(), direct_inputs);
  EXPECT_EQ(merge_op_view.GetControlInputs(), control_inputs);
}

// Create some example values for use in test inputs. Skip `StoredValue`
// because it's hard to construct and the actual kinds of values aren't
// as important as their indices for this test anyway.
static const Operator kExampleOp("example");
static const Operation kExampleOperation(nullptr, kExampleOp, {}, {});

static const Block kExampleBlock;

static const std::vector<Value> kSampleInputVectors[] = {
    std::vector<Value>{},
    {Value(Any())},
    {Value(OperationResult(kExampleOperation, "out")),
     Value(BlockArgument(kExampleBlock, "arg0")), Value(Any())}};

INSTANTIATE_TEST_SUITE_P(DecodeMergeOpTest, DecodeMergeOpTest,
                         Combine(ValuesIn(kSampleInputVectors),
                                 ValuesIn(kSampleInputVectors)));

class DecodeTagTransformTest
    : public TestWithParam<
          std::tuple<ir::Value, absl::string_view, std::vector<ir::Value>,
                     std::vector<uint64_t>>> {
 protected:
  DecodeTagTransformTest() : ir_context_(), decoder_context_(ir_context_),
  value_vec_(std::get<2>(GetParam())) {
    // The id_sequence_vec is an id sequence that may have more IDs than we
    // need for giving each value an ID. We will put a value_vec.size()
    // prefix of it into input_id_vec_ so input_id_vec_ contains only those IDs
    // associated with a value.
    const std::vector<uint64_t> &id_sequence_vec = std::get<3>(GetParam());

    input_id_vec_.reserve(value_vec_.size());
    for (uint64_t idx = 0; idx < value_vec_.size(); ++idx) {
      uint64_t id = id_sequence_vec.at(idx);
      decoder_context_.RegisterValue(id, value_vec_.at(idx));
      input_id_vec_.push_back(id);
    }
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
  const std::vector<Value> &value_vec_;
  std::vector<uint64_t> input_id_vec_;
};

TEST_P(DecodeTagTransformTest, DecodeTagTransformTest) {
  Value xformed_value = std::get<0>(GetParam());
  absl::string_view policy_rule_name = std::get<1>(GetParam());
  const Operation &tag_xform_operation =
      decoder_context_.MakeTagTransformOperation(
          xformed_value, policy_rule_name, input_id_vec_);
  testing::TagTransformOperationView tag_xform_view(tag_xform_operation);

  EXPECT_EQ(tag_xform_view.GetRuleName(), policy_rule_name);
  EXPECT_EQ(tag_xform_view.GetTransformedValue(), xformed_value);
  EXPECT_EQ(tag_xform_view.GetPreconditions(), value_vec_);
}

static const Value kSampleValues[] = {
    Value(Any()),
    Value(OperationResult(kExampleOperation, "out")),
    Value(OperationResult(kExampleOperation, "out2")),
    Value(BlockArgument(kExampleBlock, "arg0")),
    Value(BlockArgument(kExampleBlock, "arg1")),
};

static const absl::string_view kSampleRuleNames[] = {
    "clear_milliseconds",
    "SSN",
    "ApprovedWindow",
};

static const std::vector<uint64_t> kSampleIdSequences[] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {3, 90, 25, 7, 33, 49, 51, 2, 17, 400}};

INSTANTIATE_TEST_SUITE_P(DecodeTagTransformTest, DecodeTagTransformTest,
                         Combine(ValuesIn(kSampleValues),
                                 ValuesIn(kSampleRuleNames),
                                 ValuesIn(kSampleInputVectors),
                                 ValuesIn(kSampleIdSequences)));

}  // namespace raksha::frontends::sql
