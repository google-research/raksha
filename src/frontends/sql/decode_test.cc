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

#include "src/frontends/sql/decode.h"

#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/merge_operation_view.h"

namespace raksha::frontends::sql {

using ir::Attribute;
using ir::Block;
using ir::IRContext;
using ir::Operation;
using ir::Storage;
using ir::Value;
using ir::types::TypeBase;
using ir::value::OperationResult;
using ir::value::StoredValue;
using ::testing::Combine;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::ResultOf;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using ::testing::Values;
using ::testing::ValuesIn;

// A value-parameterized test fixture used for both SourceTableColumns and
// Literals. Both of these constructs are basically just expressions
// containing a string value. However, the textproto that describes them is
// slightly different, and the IR that is generated is different. This
// superclass allows us to reuse the common logic while the virtual
// `GetTexprotoFormat` allows us to construct the textproto as necessary for
// each structure.
class IdNameAndStringTest
    : public TestWithParam<std::tuple<
          uint64_t, std::optional<absl::string_view>, absl::string_view>> {
 protected:
  IdNameAndStringTest() : ir_context_(), decoder_context_(ir_context_) {}

  virtual absl::ParsedFormat<'u', 's', 's'> GetTextprotoFormat() const = 0;

  std::string GetTextproto() const {
    auto &[id, name, str] = GetParam();
    std::string name_str =
        (name.has_value()) ? absl::StrFormat(R"(name: "%s")", *name) : "";
    return absl::StrFormat(GetTextprotoFormat(), id, name_str, str);
  }

  Value GetDecodedValue() {
    Expression expr;
    EXPECT_TRUE(
        google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr))
        << "Could not decode expr";
    return DecodeExpression(expr, decoder_context_);
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

class DecodeSourceTableColumnExprTest : public IdNameAndStringTest {
  absl::ParsedFormat<'u', 's', 's'> GetTextprotoFormat() const override {
    return absl::ParsedFormat<'u', 's', 's'>(
        R"(id: %u %s source_table_column: { column_path: "%s" })");
  }
};

constexpr uint64_t kSampleIds[] = {1, 5, 1000};

constexpr std::optional<absl::string_view> kSampleExprNames[] = {
    {}, {"name1"}, {"another_name"}};

absl::string_view kStrings[] = {"MyTable.col",
                                "UserAlias",
                                "MySchema.MyTable.col2",
                                "MySchema.JoinTable.Table1.col",
                                "9",
                                "hello world!",
                                "3.1415"};

TEST_P(DecodeSourceTableColumnExprTest, DecodeSourceTableColumnExprTest) {
  auto &[id, name, str] = GetParam();
  Value result = GetDecodedValue();
  const StoredValue *stored_value = result.If<StoredValue>();
  EXPECT_THAT(stored_value, NotNull());
  const Storage &storage = stored_value->storage();
  EXPECT_EQ(storage.name(), str);
  EXPECT_EQ(storage.type().type_base().kind(), TypeBase::Kind::kPrimitive);
  EXPECT_EQ(result, decoder_context_.GetValue(id));
}

INSTANTIATE_TEST_SUITE_P(DecodeSourceTableColumnExprTest,
                         DecodeSourceTableColumnExprTest,
                         Combine(ValuesIn(kSampleIds),
                                 ValuesIn(kSampleExprNames),
                                 ValuesIn(kStrings)));

class DecodeLiteralExprTest : public IdNameAndStringTest {
  absl::ParsedFormat<'u', 's', 's'> GetTextprotoFormat() const override {
    return absl::ParsedFormat<'u', 's', 's'>(
        R"(id: %u %s literal: { literal_str: "%s" })");
  }
};

TEST_P(DecodeLiteralExprTest, DecodeLiteralExprTest) {
  auto &[id, name, str] = GetParam();
  const Value &result = GetDecodedValue();
  const OperationResult *operation_result = result.If<OperationResult>();
  EXPECT_THAT(operation_result, NotNull());
  const Operation &operation = operation_result->operation();
  const Block &top_block = decoder_context_.BuildTopLevelBlock();
  EXPECT_EQ(operation.parent(), &top_block);
  EXPECT_THAT(operation.impl_module(), IsNull());
  EXPECT_EQ(operation.op().name(), DecoderContext::kSqlLiteralOpName);
  EXPECT_THAT(operation.inputs(), IsEmpty());

  // Check that attributes have exactly the name given.
  EXPECT_THAT(
      operation.attributes(),
      UnorderedElementsAre(Pair(
          DecoderContext::kLiteralStrAttrName,
          ResultOf([](Attribute attr) { return attr->ToString(); }, Eq(str)))));
  EXPECT_EQ(result, decoder_context_.GetValue(id));
}

INSTANTIATE_TEST_SUITE_P(DecodeLiteralExprTest, DecodeLiteralExprTest,
                         Combine(ValuesIn(kSampleIds),
                                 ValuesIn(kSampleExprNames),
                                 ValuesIn(kStrings)));

struct TextprotoDeathMessagePair {
  std::string textproto;
  std::string death_message;
};

class DecodeExprDeathTest : public TestWithParam<TextprotoDeathMessagePair> {
 protected:
  DecodeExprDeathTest() : ir_context_(), decoder_context_(ir_context_) {}

  Expression GetExpr() {
    Expression expr;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        GetParam().textproto, &expr))
        << "Could not decode expr";
    return expr;
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

using DecodeExprIdCollisionDeathTest = DecodeExprDeathTest;

TEST_P(DecodeExprIdCollisionDeathTest, DecodeExprIdCollisionDeathTest) {
  Expression expr = GetExpr();
  DecodeExpression(expr, decoder_context_);
  EXPECT_DEATH({ DecodeExpression(expr, decoder_context_); },
               GetParam().death_message);
}

INSTANTIATE_TEST_SUITE_P(
    DecodeExprIdCollisionDeathTest, DecodeExprIdCollisionDeathTest,
    Values(TextprotoDeathMessagePair{
        .textproto =
            R"(id: 1 source_table_column: { column_path: "table1.col" } )",
        .death_message = "id_to_value map has more than one value associated "
                         "with the id 1."}));

TEST_P(DecodeExprDeathTest, DecodeExprDeathTest) {
  Expression expr = GetExpr();
  EXPECT_DEATH({ DecodeExpression(expr, decoder_context_); },
               GetParam().death_message);
}

const TextprotoDeathMessagePair kTextprotoDeathMessagePairs[] = {
    {.textproto =
         R"(id: 0 source_table_column: { column_path: "table1.col" } )",
     .death_message = "Required field id was not present in Expression."},
    {.textproto = R"(source_table_column: { column_path: "table1.col" } )",
     .death_message = "Required field id was not present in Expression."},
    {.textproto = R"(id: 1)",
     .death_message = "Required field expr_variant not set."},
    {.textproto = R"(id: 1 merge_operation: { })",
     .death_message = "Each MergeOperation is expected to have at least one "
                      "non-control input."},
};

INSTANTIATE_TEST_SUITE_P(DecodeExprDeathTest, DecodeExprDeathTest,
                         ValuesIn(kTextprotoDeathMessagePairs));

// We test the decoding of the child `Operation`s in other tests. We also
// ensure that we can correctly make a `MergeOperation` from two vectors of
// inputs in another test. Therefore, this is strictly about whether we can
// decode the `MergeOperation` proto. This test accepts 4 parameters: the id,
// the name, the number of direct inputs, and the number of control inputs.
// We will generate the given number of `Literal` values on the input proto.
class DecodeMergeOpTest
    : public TestWithParam<
          std::tuple<std::optional<absl::string_view>, uint64_t, uint64_t>> {
 protected:
  DecodeMergeOpTest() : ir_context_(), decoder_context_(ir_context_) {}

  std::string GenerateLiteralListContentsProto(uint64_t len,
                                               uint64_t id_start) const {
    std::vector<std::string> literal_vec;
    for (uint64_t id = id_start; id < id_start + len; ++id) {
      literal_vec.push_back(
          absl::StrFormat(R"({ id: %u literal: { literal_str: "lit" } })", id));
    }
    return absl::StrJoin(literal_vec, ",");
  }

  std::string GetTextproto() const {
    auto &[name, direct_size, control_size] = GetParam();
    std::string name_str =
        (name.has_value()) ? absl::StrFormat(R"(name: "%s")", *name) : "";
    // Start from 2 so that we can assign 1 to the top level MergeOperation.
    std::string direct_input_list_contents =
        GenerateLiteralListContentsProto(direct_size, 2);
    std::string control_input_list_contents =
        GenerateLiteralListContentsProto(control_size, 2 + direct_size);
    return absl::StrFormat(
        "id: 1 %s merge_operation: { inputs: [ %s ] control_inputs: [ %s ] }",
        name_str, direct_input_list_contents, control_input_list_contents);
  }

  Value GetDecodedValue() {
    Expression expr;
    EXPECT_TRUE(
        google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr))
        << "Could not decode expr";
    return DecodeExpression(expr, decoder_context_);
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

TEST_P(DecodeMergeOpTest, DecodeMergeOpTest) {
  auto &[opt_name, direct_size, control_size] = GetParam();

  Expression expr;
  EXPECT_TRUE(
      google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr))
      << "Could not decode expr";
  Value value = DecodeExpression(expr, decoder_context_);
  EXPECT_EQ(value, decoder_context_.GetValue(1));
  const Block &top_level_block = decoder_context_.BuildTopLevelBlock();
  const OperationResult *op_result = value.If<OperationResult>();
  EXPECT_THAT(op_result, NotNull());
  EXPECT_EQ(op_result->name(), DecoderContext::kDefaultOutputName);
  const Operation &op = op_result->operation();
  EXPECT_EQ(op.parent(), &top_level_block);

  testing::MergeOperationView merge_operation_view(op);

  EXPECT_THAT(merge_operation_view.GetDirectInputs().size(), direct_size);
  EXPECT_THAT(merge_operation_view.GetControlInputs().size(), control_size);
}

static constexpr uint64_t kSampleControlInputLengths[] = {0, 1, 5, 10};
// 0 is not allowed for direct input lengths. Array is otherwise the same as
// the control input lengths.
static constexpr uint64_t kSampleDirectInputLengths[] = {1, 5, 10};

INSTANTIATE_TEST_SUITE_P(DecodeMergeOpTest, DecodeMergeOpTest,
                         Combine(ValuesIn(kSampleExprNames),
                                 ValuesIn(kSampleDirectInputLengths),
                                 ValuesIn(kSampleControlInputLengths)));

}  // namespace raksha::frontends::sql
