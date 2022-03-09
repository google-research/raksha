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

#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"

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

}  // namespace raksha::frontends::sql
