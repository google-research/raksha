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

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/map_iter.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/merge_operation_view.h"
#include "src/frontends/sql/testing/tag_transform_operation_view.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

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
using testing::MergeOperationView;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::ResultOf;
using testing::TagTransformOperationView;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using testing::UnwrapDefaultOperationResult;
using ::testing::Values;
using ::testing::ValuesIn;
using utils::MapIter;

class DecodeTagTransformTest
    : public ::testing::TestWithParam<
          std::tuple<uint64_t, absl::string_view,
                     std::pair<absl::string_view, std::vector<uint64_t>>>> {};

TEST_P(DecodeTagTransformTest, DecodeTagTransformTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  // Construct the test textproto and parse it.
  constexpr absl::string_view kTagTransformTextprotoFormat = R"(
id: %u
tag_transform : {
  transform_rule_name: "%s"
  tag_precondition_ids: [ %s ]
  transformed_node: { %s } })";
  const auto &[id, rule_name, value_textproto_and_ids] = GetParam();
  const auto &[value_textproto, value_ids] = value_textproto_and_ids;

  Expression expr;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
      absl::StrFormat(kTagTransformTextprotoFormat, id, rule_name,
                      absl::StrJoin(value_ids, ", "), value_textproto),
      &expr));
  Value decoded_value = DecodeExpression(expr, decoder_context);

  std::vector<Value> expected_values =
      MapIter<Value>(value_ids, [&decoder_context](uint64_t current_id) {
        return decoder_context.GetValue(current_id);
      });

  EXPECT_EQ(decoded_value, decoder_context.GetValue(id));
  TagTransformOperationView tag_xform_view(
      UnwrapDefaultOperationResult(decoded_value));
  EXPECT_EQ(tag_xform_view.GetRuleName(), rule_name);
  // Dealing with the internal structure of the sub-value is the job of
  // another test, so we don't inspect it here. What does matter is ensuring
  // that the value vec that we got corresponds to the ID vec that we took
  // from the textproto.
  EXPECT_EQ(tag_xform_view.GetPreconditions(), expected_values);
}

static const std::pair<absl::string_view, std::vector<uint64_t>>
    kValueTextprotoAndPreconditionIdVecs[] = {
        {R"(id: 4 literal: { literal_str: "foo" })", {4}},
        {R"(id: 6 merge_operation: { inputs: { id: 3 literal: { literal_str: "5" } } })",
         {6, 3}},
        {R"(id: 100 source_table_column: { column_path: "Table1.col" })",
         {100}},
};

static const uint64_t kExampleIds[] = {7, 108, 300};
static const absl::string_view kExampleRuleNames[] = {"rule1", "clearLowBits",
                                                      "SQL.Identity"};

INSTANTIATE_TEST_SUITE_P(
    DecodeTagTransformTest, DecodeTagTransformTest,
    Combine(ValuesIn(kExampleIds), ValuesIn(kExampleRuleNames),
            ValuesIn(kValueTextprotoAndPreconditionIdVecs)));

}  // namespace

}  // namespace raksha::frontends::sql
