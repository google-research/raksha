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
#include "src/common/utils/map_iter.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"

namespace raksha::frontends::sql {

using ::testing::Combine;
using ::testing::ElementsAreArray;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAreArray;
using ::testing::Values;
using ::testing::ValuesIn;

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
using utils::MapIter;

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
  const Operation &op = decoder_context.MakeLiteralOperation(literal_str);

  const Block &top_level_block = decoder_context.BuildTopLevelBlock();

  EXPECT_THAT(op.parent(), &top_level_block);
  EXPECT_THAT(op.impl_module(), IsNull());

  const LiteralOp *literal_op = SqlOp::GetIf<LiteralOp>(op);
  ASSERT_NE(literal_op, nullptr);
  EXPECT_EQ(literal_op->GetLiteralString(), literal_str);
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

  uint64_t next_id = 1;
  auto register_value_and_get_id = [&decoder_context,
                                    &next_id](Value value) mutable {
    uint64_t current_id = next_id++;
    decoder_context.RegisterValue(current_id, value);
    return current_id;
  };
  auto direct_input_ids =
      MapIter<uint64_t>(direct_inputs, register_value_and_get_id);
  auto control_input_ids =
      MapIter<uint64_t>(control_inputs, register_value_and_get_id);

  const Operation &op =
      decoder_context.MakeMergeOperation(direct_input_ids, control_input_ids);
  const Block &top_block = decoder_context.BuildTopLevelBlock();

  EXPECT_EQ(op.parent(), &top_block);

  auto merge_op = SqlOp::GetIf<MergeOp>(op);
  ASSERT_NE(merge_op, nullptr);
  // Test the vectors for equivalence.
  EXPECT_THAT(merge_op->GetDirectInputs(), ElementsAreArray(direct_inputs));
  EXPECT_THAT(merge_op->GetControlInputs(), ElementsAreArray(control_inputs));
}

// Create some example values for use in test inputs. Skip `StoredValue`
// because it's hard to construct and the actual kinds of values aren't
// as important as their indices for this test anyway.
static const Operator kExampleOp("example");
static const Operation kExampleOperation(nullptr, kExampleOp, {}, {});

static const Block kExampleBlock;

static const std::vector<Value> kSampleInputVectors[] = {
    {Value(Any())},
    {Value(OperationResult(kExampleOperation, "out")),
     Value(BlockArgument(kExampleBlock, "arg0")), Value(Any())}};

INSTANTIATE_TEST_SUITE_P(NonEmptyValues, DecodeMergeOpTest,
                         Combine(ValuesIn(kSampleInputVectors),
                                 ValuesIn(kSampleInputVectors)));
INSTANTIATE_TEST_SUITE_P(EmptyDirectInputs, DecodeMergeOpTest,
                         Combine(Values(ir::ValueList({})),
                                 ValuesIn(kSampleInputVectors)));
INSTANTIATE_TEST_SUITE_P(EmptyControlInputs, DecodeMergeOpTest,
                         Combine(ValuesIn(kSampleInputVectors),
                                 Values(ir::ValueList({}))));
class DecodeTagTransformTest
    : public TestWithParam<
          std::tuple<ir::Value, absl::string_view, std::vector<ir::Value>,
                     std::vector<uint64_t>, std::vector<absl::string_view>>> {};

TEST_P(DecodeTagTransformTest, DecodeTagTransformTest) {
  const auto &[xformed_value, policy_rule_name, precondition_values,
               id_sequence, precondition_name_sequence] = GetParam();

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  // The id_sequence and precondition_name_sequence each may have more entries
  // need for associating each input with a name and ID. We will select the
  // first precondition_values.size() prefix of each to build our name to ID
  // map.
  common::containers::HashMap<std::string, uint64_t>
      precondition_name_to_id_map;
  precondition_name_to_id_map.reserve(precondition_values.size());
  uint64_t max_id = 0;
  for (uint64_t idx = 0; idx < precondition_values.size(); ++idx) {
    uint64_t id = id_sequence.at(idx);
    max_id = std::max(max_id, id);
    precondition_name_to_id_map.insert(
        {std::string(precondition_name_sequence.at(idx)), id});
    decoder_context.RegisterValue(id, precondition_values.at(idx));
  }

  uint64_t xformed_value_id = max_id + 1;
  decoder_context.RegisterValue(xformed_value_id, xformed_value);

  // Setup has finished. Create the tag transform operation and check its
  // properties.
  const Operation &tag_xform_operation =
      decoder_context.MakeTagTransformOperation(
          xformed_value_id, policy_rule_name, precondition_name_to_id_map);

  const TagTransformOp *tag_transform_op =
      SqlOp::GetIf<TagTransformOp>(tag_xform_operation);
  ASSERT_NE(tag_transform_op, nullptr);
  common::containers::HashMap<std::string, Value>
      precondition_name_to_value_map;
  precondition_name_to_value_map.reserve(precondition_name_to_id_map.size());
  for (const auto &[name, id] : precondition_name_to_id_map) {
    precondition_name_to_value_map.insert({name, decoder_context.GetValue(id)});
  }

  EXPECT_EQ(tag_transform_op->GetRuleName(), policy_rule_name);
  EXPECT_EQ(tag_transform_op->GetTransformedValue(), xformed_value);
  EXPECT_THAT(tag_transform_op->GetPreconditions(),
              UnorderedElementsAreArray(precondition_name_to_value_map));
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

static const std::vector<absl::string_view> kSamplePreconditionNames[] = {
    {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"},
    // These are the first 10 words of Frakenstein by Mary
    // Wollstonecraft Shelley, and are public domain. Retrieved via Project
    // Gutenberg (https://www.gutenberg.org/files/84/84-h/84-h.htm#letter1).
    {
        "You",
        "will",
        "rejoice",
        "to",
        "hear",
        "that",
        "no",
        "disaster",
        "has",
        "accompanied",
    },
};

INSTANTIATE_TEST_SUITE_P(DecodeTagTransformTest, DecodeTagTransformTest,
                         Combine(ValuesIn(kSampleValues),
                                 ValuesIn(kSampleRuleNames),
                                 ValuesIn(kSampleInputVectors),
                                 ValuesIn(kSampleIdSequences),
                                 ValuesIn(kSamplePreconditionNames)));

}  // namespace raksha::frontends::sql
