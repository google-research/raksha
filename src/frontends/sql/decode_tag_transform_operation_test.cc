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

#include "google/protobuf/text_format.h"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/map_iter.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

using ir::IRContext;
using ir::Value;
using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAreArray;
using testing::UnwrapDefaultOperationResult;
using ::testing::ValuesIn;

class DecodeTagTransformTest
    : public ::testing::TestWithParam<
          std::tuple<uint64_t, absl::string_view,
                     std::vector<absl::string_view>, std::vector<uint64_t>>> {};

TEST_P(DecodeTagTransformTest, DecodeTagTransformTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  // Construct the test textproto and parse it.
  constexpr absl::string_view kTagTransformTextprotoFormat = R"(
    {
      id: 100
      expression: {
        tag_transform : {
          transform_rule_name: "%s"
          tag_preconditions: [ %s ]
          transformed_node: %u } } })";
  const auto &[transformed_node_id, rule_name, precondition_name_sequence,
               value_ids] = GetParam();

  absl::flat_hash_map<absl::string_view, uint64_t> precondition_name_to_id;
  std::vector<uint64_t> ids_to_be_filled;
  for (uint64_t i = 0; i < value_ids.size(); ++i) {
    uint64_t current_precondition_id = value_ids.at(i);
    precondition_name_to_id.insert(
        {precondition_name_sequence.at(i), current_precondition_id});
    ids_to_be_filled.push_back(current_precondition_id);
  }
  ids_to_be_filled.push_back(transformed_node_id);

  std::string exprArenaTextproto =
      testing::CreateExprArenaTextprotoWithLiteralsPrefix(
          absl::StrFormat(kTagTransformTextprotoFormat, rule_name,
                          absl::StrJoin(precondition_name_to_id, ", ",
                                        [](std::string *out, auto pair) {
                                          return absl::StrAppend(
                                              out,
                                              absl::StrFormat(
                                                  R"({ key: "%s" value: %u })",
                                                  pair.first, pair.second));
                                        }),
                          transformed_node_id),
          ids_to_be_filled);

  ExpressionArena exprArena;
  EXPECT_TRUE(google3_proto_compat::TextFormat::ParseFromString(
      exprArenaTextproto, &exprArena));
  Value decoded_value = DecodeExpressionArena(exprArena, decoder_context);

  const TagTransformOp *tag_transform_op =
      SqlOp::GetIf<TagTransformOp>(UnwrapDefaultOperationResult(decoded_value));
  ASSERT_NE(tag_transform_op, nullptr);
  EXPECT_EQ(tag_transform_op->GetRuleName(), rule_name);
  // Dealing with the internal structure of the sub-value is the job of
  // another test, so we don't inspect it here. What does matter is ensuring
  // that the value vec that we got corresponds to the ID vec that we took
  // from the textproto.
  auto expected_name_to_value_vec =
      utils::MapIter<std::pair<std::string, Value>>(
          precondition_name_to_id, [&decoder_context](auto current_pair) {
            return std::make_pair(
                std::string(current_pair.first),
                decoder_context.GetValue(current_pair.second));
          });
  auto expected_name_to_value_map = absl::flat_hash_map<std::string, Value>(
      expected_name_to_value_vec.begin(), expected_name_to_value_vec.end());
  EXPECT_THAT(tag_transform_op->GetPreconditions(),
              UnorderedElementsAreArray(expected_name_to_value_map));
}

static const std::vector<uint64_t> kPreconditionIdVecs[] = {
    {4},
    {1, 2, 3},
    {6, 3, 17},
    {20},
};

static const uint64_t kExampleIds[] = {7, 21, 19};
static const absl::string_view kExampleRuleNames[] = {"rule1", "clearLowBits",
                                                      "SQL.Identity"};

static const std::vector<absl::string_view>
    kExamplePreconditionNameSequences[] = {
        {"a", "b", "c"},
        {"1", "2", "3"},
        {"foo", "bar", "baz"},
};

INSTANTIATE_TEST_SUITE_P(DecodeTagTransformTest, DecodeTagTransformTest,
                         Combine(ValuesIn(kExampleIds),
                                 ValuesIn(kExampleRuleNames),
                                 ValuesIn(kExamplePreconditionNameSequences),
                                 ValuesIn(kPreconditionIdVecs)));

}  // namespace

}  // namespace raksha::frontends::sql
