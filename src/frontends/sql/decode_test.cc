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

namespace raksha::frontends::sql {

using testing::Eq;
using testing::IsEmpty;
using testing::IsNull;
using testing::NotNull;
using testing::Pair;
using testing::ResultOf;
using testing::TestWithParam;
using testing::UnorderedElementsAre;
using testing::Values;
using testing::ValuesIn;

using ir::types::TypeBase;

using ir::Attribute;
using ir::IRContext;
using ir::Operation;
using ir::Storage;
using ir::Value;
using ir::value::OperationResult;
using ir::value::StoredValue;

class DecodeSourceTableColumnTest : public TestWithParam<absl::string_view> {};

TEST_P(DecodeSourceTableColumnTest, DecodeSourceTableColumnTest) {
  absl::string_view column_path = GetParam();
  std::string textproto = absl::StrFormat(R"(column_path: "%s")", column_path);
  SourceTableColumn col_proto;

  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(textproto, &col_proto))
      << "Could not decode SourceTableColumn";

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  Value result = DecodeSourceTableColumn(col_proto, decoder_context);
  const StoredValue *stored_value = result.If<StoredValue>();
  EXPECT_THAT(stored_value, NotNull());
  const Storage &storage = stored_value->storage();
  EXPECT_EQ(storage.name(), column_path);
  EXPECT_EQ(storage.type().type_base().kind(), TypeBase::Kind::kPrimitive);
}

absl::string_view kSourceColumnPaths[] = {
    "MyTable.col",
    "UserAlias",
    "MySchema.MyTable.col2",
    "MySchema.JoinTable.Table1.col",
};

INSTANTIATE_TEST_SUITE_P(DecodeSourceTableColumnTest,
                         DecodeSourceTableColumnTest,
                         ValuesIn(kSourceColumnPaths));

class DecodeLiteralTest : public TestWithParam<absl::string_view> {};

TEST_P(DecodeLiteralTest, DecodeLiteralTest) {
  absl::string_view literal_str = GetParam();
  std::string textproto = absl::StrFormat(R"(literal_str: "%s")", literal_str);
  Literal literal_proto;

  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(textproto, &literal_proto))
      << "Could not decode Literal";

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  ir::Value result = DecodeLiteral(literal_proto, decoder_context);
  const OperationResult *op_result = result.If<OperationResult>();
  const ir::Block &top_level_block = decoder_context.BuildTopLevelBlock();
  EXPECT_THAT(op_result, NotNull());
  EXPECT_EQ(op_result->name(), "out");
  const Operation &operation = op_result->operation();
  EXPECT_THAT(operation.parent(), &top_level_block);
  EXPECT_THAT(operation.impl_module(), IsNull());
  EXPECT_EQ(operation.op().name(), DecoderContext::kSqlLiteralOpName);
  EXPECT_THAT(operation.inputs(), IsEmpty());

  // Check that attributes have exactly the name given.
  EXPECT_THAT(operation.attributes(),
              UnorderedElementsAre(
                  Pair(DecoderContext::kLiteralStrAttrName,
                       ResultOf([](Attribute attr) { return attr->ToString(); },
                                Eq(literal_str)))));
}

absl::string_view kLiteralStrs[] = {
    "100", "0", "3.1415", "hello world!", "null",
};

INSTANTIATE_TEST_SUITE_P(DecodeLiteralTest, DecodeLiteralTest,
                         ValuesIn(kLiteralStrs));

TEST(DecodeSourceTableColumnExprTest, DecodeSourceTableColumnExprTest) {
  const std::string kTextproto =
      R"(id: 1 name: "alias" source_table_column: { column_path: "table1.col" })";
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  Expression expr;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(kTextproto, &expr))
      << "Could not decode expr";
  const ir::Value &result = DecodeExpression(expr, decoder_context);
  const StoredValue *stored_value = result.If<StoredValue>();
  EXPECT_THAT(stored_value, testing::NotNull());
  const Storage &storage = stored_value->storage();
  EXPECT_EQ(storage.name(), "table1.col");
  EXPECT_EQ(storage.type().type_base().kind(), TypeBase::Kind::kPrimitive);
  EXPECT_EQ(&result, &decoder_context.GetValue(1));
}

TEST(DecodeLiteralExprTest, DecodeLiteralExprTest) {
  const std::string kTextproto =
      R"(id: 5 name: "alias" literal: { literal_str: "9" } )";
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  Expression expr;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(kTextproto, &expr))
      << "Could not decode expr";
  const ir::Value &result = DecodeExpression(expr, decoder_context);
  const OperationResult *operation_result = result.If<OperationResult>();
  EXPECT_THAT(operation_result, testing::NotNull());
  const Operation &operation = operation_result->operation();
  const ir::Block &top_level_block = decoder_context.BuildTopLevelBlock();
  EXPECT_EQ(operation.parent(), &top_level_block);
  EXPECT_THAT(operation.impl_module(), IsNull());
  EXPECT_EQ(operation.op().name(), DecoderContext::kSqlLiteralOpName);
  EXPECT_THAT(operation.inputs(), IsEmpty());

  // Check that attributes have exactly the name given.
  EXPECT_THAT(
      operation.attributes(),
      UnorderedElementsAre(Pair(
          DecoderContext::kLiteralStrAttrName,
          ResultOf([](Attribute attr) { return attr->ToString(); }, Eq("9")))));
  EXPECT_EQ(&result, &decoder_context.GetValue(5));
}

TEST(DecodeExprIdCollisionDeathTest, DecodeExprIdCollisionDeathTest) {
  const std::string kTextproto =
      R"(id: 1 source_table_column: { column_path: "table1.col" } )";
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  Expression expr;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(kTextproto, &expr))
      << "Could not decode expr";
  DecodeExpression(expr, decoder_context);
  EXPECT_DEATH(
      { DecodeExpression(expr, decoder_context); },
      "id_to_value map has more than one value associated with the id 1.");
}

struct TextprotoDeathMessagePair {
  std::string textproto;
  std::string death_message;
};

class DecodeExprDeathTest
    : public testing::TestWithParam<TextprotoDeathMessagePair> {};

TEST_P(DecodeExprDeathTest, DecodeExprDeathTest) {
  const TextprotoDeathMessagePair &param = GetParam();
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  Expression expr;
  ASSERT_TRUE(
      google::protobuf::TextFormat::ParseFromString(param.textproto, &expr))
      << "Could not decode expr";
  EXPECT_DEATH({ DecodeExpression(expr, decoder_context); },
               param.death_message);
}

const TextprotoDeathMessagePair kTextprotoDeathMessagePairs[] = {
    {.textproto =
         R"(id: 0 source_table_column: { column_path: "table1.col" } )",
     .death_message = "Required field id was not present in Expression."},
    {.textproto = R"(source_table_column: { column_path: "table1.col" } )",
     .death_message = "Required field id was not present in Expression."},
    {.textproto = R"(id: 1)",
     .death_message = "Required field expr_variant not set."},
};

INSTANTIATE_TEST_SUITE_P(DecodeExprDeathTest, DecodeExprDeathTest,
                         testing::ValuesIn(kTextprotoDeathMessagePairs));

}  // namespace raksha::frontends::sql
