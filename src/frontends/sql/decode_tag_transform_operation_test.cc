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
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/literal_operation_view.h"
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
using testing::LiteralOperationView;
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

// This struct carries a textproto string describing a value and a lambda
// that, given that value, verifies that its properties are as expected. This
// allows us to combine the various pieces of a `TagTransform` and flexibly
// verify that the value to which the transform is applied is as we expect.
struct ValueTextprotoAndDeconstruction {
  absl::string_view textproto;
  std::function<void(Value)> deconstruction;
};

class DecodeTagTransformTest
    : public ::testing::TestWithParam<std::tuple<
          uint64_t, absl::string_view, ValueTextprotoAndDeconstruction>> {
 protected:
  DecodeTagTransformTest() : ir_context_(), decoder_context_(ir_context_) {}

  static absl::ParsedFormat<'u', 's', 's'> GetProtoFormat() {
    return absl::ParsedFormat<'u', 's', 's'>(R"(
id: %u
tag_transform : { transform_rule_name: "%s" transformed_node: { %s } })");
  }

  std::string GetTextproto() const {
    const auto &[id, rule_name, value_textproto_and_deconstruction] =
        GetParam();
    return absl::StrFormat(GetProtoFormat(), id, rule_name,
                           value_textproto_and_deconstruction.textproto);
  }

  Value GetValue() {
    Expression expr;
    EXPECT_TRUE(
        google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr));
    return DecodeExpression(expr, decoder_context_);
  }

  std::function<void(Value)> GetDeconstructionFunction() const {
    return std::get<2>(GetParam()).deconstruction;
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

TEST_P(DecodeTagTransformTest, DecodeTagTransformTest) {
  const auto &[id, rule_name, value_textproto_and_deconstruction] = GetParam();

  Value value = GetValue();

  EXPECT_EQ(value, decoder_context_.GetValue(id));

  TagTransformOperationView tag_xform_view(UnwrapDefaultOperationResult(value));
  EXPECT_EQ(tag_xform_view.GetRuleName(), rule_name);
  value_textproto_and_deconstruction.deconstruction(
      tag_xform_view.GetTransformedValue());
}

static const ValueTextprotoAndDeconstruction kValuesAndDeconstructions[] = {
    {.textproto = R"(id: 4 literal: { literal_str: "foo" })",
     .deconstruction =
         [](Value val) {
           const LiteralOperationView literal_operation_view(
               UnwrapDefaultOperationResult(val));
           EXPECT_EQ(literal_operation_view.GetLiteralStr(), "foo");
         }},
    {.textproto =
         R"(id: 6 merge_operation: { inputs: { id: 3 literal: { literal_str: "5" } } })",
     .deconstruction = [](Value val) {
       const MergeOperationView merge_operation_view(
           UnwrapDefaultOperationResult(val));
       EXPECT_THAT(merge_operation_view.GetControlInputs(), IsEmpty());
       const std::vector<Value> merged_values =
           merge_operation_view.GetDirectInputs();
       EXPECT_EQ(merged_values.size(), 1);
       Value merged_val = merged_values.at(0);
       const LiteralOperationView literal(
           UnwrapDefaultOperationResult(merged_val));
       EXPECT_EQ(literal.GetLiteralStr(), "5");
     }}};

static const uint64_t kExampleIds[] = {7, 108, 300};
static const absl::string_view kExampleRuleNames[] = {"rule1", "clearLowBits",
                                                      "SQL.Identity"};

INSTANTIATE_TEST_SUITE_P(DecodeTagTransformTest, DecodeTagTransformTest,
                         Combine(ValuesIn(kExampleIds),
                                 ValuesIn(kExampleRuleNames),
                                 ValuesIn(kValuesAndDeconstructions)));

}  // namespace

}  // namespace raksha::frontends::sql
