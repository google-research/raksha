//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
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
//-----------------------------------------------------------------------------

#include "src/analysis/taint/inference_rules.h"

#include "absl/strings/string_view.h"
#include "src/analysis/taint/inference_rule.h"
#include "src/common/testing/gtest.h"
#include "src/ir/module.h"

namespace raksha::analysis::taint {
namespace {

using ::testing::Eq;

InferenceRules::OutputRules test_action_rules[] = {
    {{0,
      {InferenceRule{.conclusion = CopyInput{.input_index = 0},
                     .premises = {}}}},
     {1,
      {InferenceRule{
          .conclusion = CopyInput{.input_index = 1},
          .premises = {InputHasIntegrity{.input_index = 1, .tag = 0}}}}}},
    {{1,
      {InferenceRule{
          .conclusion =
              ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity, .tag = 0},
          .premises = {InputHasIntegrity{.input_index = 2, .tag = 3}}}}}}};

class InferenceRulesTest : public ::testing::Test {
 public:
  InferenceRulesTest()
      : plus_operator_("core.plus"),
        minus_operator_("core.minus"),
        plus_(nullptr, plus_operator_, {}, {}),
        minus_(nullptr, plus_operator_, {}, {}),
        unregistered_op_(nullptr, plus_operator_, {}, {}),
        tag_a_(0),
        tag_b_(1),
        rules_(
            /*action_rules=*/{{"plus_action", test_action_rules[0]},
                              {"minus_action", test_action_rules[1]}},
            /*actions=*/{{&plus_, "plus_action"}, {&minus_, "minus_action"}},
            /*tags=*/{{"A", tag_a_}, {"B", tag_b_}}, /*tag_names=*/{"A", "B"}) {
  }

  const InferenceRules& rules() const { return rules_; }
  const ir::Operation& plus() const { return plus_; }
  const ir::Operation& minus() const { return minus_; }
  const ir::Operation& unregistered_op() const { return unregistered_op_; }
  TagId tag_a() const { return tag_a_; }
  TagId tag_b() const { return tag_b_; }

 private:
  ir::Operator plus_operator_;
  ir::Operator minus_operator_;
  ir::Operation plus_;
  ir::Operation minus_;
  ir::Operation unregistered_op_;
  TagId tag_a_;
  TagId tag_b_;
  InferenceRules rules_;
};

TEST_F(InferenceRulesTest, GetTagNameReturnsCorrectValue) {
  EXPECT_THAT(rules().GetTagName(tag_a()), Eq("A"));
  EXPECT_THAT(rules().GetTagName(tag_b()), Eq("B"));
}

TEST_F(InferenceRulesTest, GetTagNameReturnsNullIfTagIdUnknown) {
  EXPECT_THAT(rules().GetTagName(10), Eq(std::nullopt));
}

TEST_F(InferenceRulesTest, GetTagIdReturnsCorrectId) {
  EXPECT_THAT(rules().GetTagId("A"), Eq(tag_a()));
  EXPECT_THAT(rules().GetTagId("B"), Eq(tag_b()));
}

TEST_F(InferenceRulesTest, GetTagIdReturnsNullIfTagIdUnknown) {
  EXPECT_THAT(rules().GetTagId("C"), Eq(std::nullopt));
}

TEST_F(InferenceRulesTest, GetOutputRulesForActionReturnsCorrectValue) {
  EXPECT_THAT(rules().GetOutputRulesForAction("plus_action"),
              Eq(std::make_optional(test_action_rules[0])));
  EXPECT_THAT(rules().GetOutputRulesForAction("minus_action"),
              Eq(std::make_optional(test_action_rules[1])));
}

TEST_F(InferenceRulesTest, GetOutputRulesForActionReturnsNullForUnknownAction) {
  EXPECT_THAT(rules().GetOutputRulesForAction("action1"), Eq(std::nullopt));
}

TEST_F(InferenceRulesTest, GetOutputRulesForOperationReturnsCorrectValue) {
  EXPECT_THAT(rules().GetOutputRulesForOperation(plus()),
              Eq(std::make_optional(test_action_rules[0])));
  EXPECT_THAT(rules().GetOutputRulesForOperation(minus()),
              Eq(std::make_optional(test_action_rules[1])));
}

TEST_F(InferenceRulesTest,
       GetOutputRulesForOperationReturnsNullForUnknownOperation) {
  EXPECT_THAT(rules().GetOutputRulesForOperation(unregistered_op()),
              Eq(std::nullopt));
}

}  // namespace
}  // namespace raksha::analysis::taint
