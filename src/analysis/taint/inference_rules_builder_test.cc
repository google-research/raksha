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

#include "src/analysis/taint/inference_rules_builder.h"

#include "absl/strings/string_view.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/common/testing/gtest.h"

namespace raksha::analysis::taint {
namespace {

using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

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

TEST(InferenceRulesBuilderTest, GetOrCreateTagIdReturnsConsistentIds) {
  InferenceRulesBuilder builder;
  TagId tag_a = builder.GetOrCreateTagId("A");
  TagId tag_b = builder.GetOrCreateTagId("B");

  EXPECT_THAT(builder.GetOrCreateTagId("A"), Eq(tag_a));
  EXPECT_THAT(builder.GetOrCreateTagId("B"), Eq(tag_b));
  EXPECT_NE(tag_a, tag_b);
}

TEST(InferenceRulesBuilderTest, GetOrCreateTagIdStoresTheNamesAndTheMapping) {
  InferenceRulesBuilder builder;
  TagId tag_a = builder.GetOrCreateTagId("A");
  TagId tag_b = builder.GetOrCreateTagId("B");

  InferenceRules rules = builder.Build();
  EXPECT_THAT(rules.tag_names(), UnorderedElementsAre("A", "B"));
  EXPECT_THAT(rules.tags(), UnorderedElementsAre(std::make_pair("A", tag_a),
                                                 std::make_pair("B", tag_b)));
}

TEST(InferenceRulesBuilderTest, AddOutputRulesForActionRemembersTheRules) {
  InferenceRulesBuilder builder;
  builder.AddOutputRulesForAction("action1", test_action_rules[0]);
  builder.AddOutputRulesForAction("action2", test_action_rules[1]);

  InferenceRules rules = builder.Build();
  EXPECT_THAT(
      rules.action_rules(),
      UnorderedElementsAre(std::make_pair("action1", test_action_rules[0]),
                           std::make_pair("action2", test_action_rules[1])));
  EXPECT_THAT(rules.actions(), IsEmpty());
  EXPECT_THAT(rules.tag_names(), IsEmpty());
  EXPECT_THAT(rules.tags(), IsEmpty());
}

TEST(InferenceRulesBuilderDeathTest,
     AddOutputRulesForActionMultipleTimesFails) {
  InferenceRulesBuilder builder;
  builder.AddOutputRulesForAction("action1", test_action_rules[0]);
  EXPECT_DEATH(builder.AddOutputRulesForAction("action1", test_action_rules[1]),
               "Duplicate inference rule inserted for 'action1'");
}

TEST(InferenceRulesBuilderTest, MarkOperationAsActionRemembersTheAssociation) {
  InferenceRulesBuilder builder;

  ir::Operator plus_operator("core.plus");
  ir::Operator minus_operator("core.minus");
  ir::Operation plus(nullptr, plus_operator, {}, {});
  ir::Operation minus(nullptr, minus_operator, {}, {});

  builder.MarkOperationAsAction(plus, "plus_action");
  builder.MarkOperationAsAction(minus, "minus_action");

  InferenceRules rules = builder.Build();
  EXPECT_THAT(rules.action_rules(), IsEmpty());
  EXPECT_THAT(rules.actions(),
              UnorderedElementsAre(std::make_pair(&plus, "plus_action"),
                                   std::make_pair(&minus, "minus_action")));
  EXPECT_THAT(rules.tag_names(), IsEmpty());
  EXPECT_THAT(rules.tags(), IsEmpty());
}

TEST(InferenceRulesBuilderDeathTest,
     MarkingAnOperationAsActionMultipleTimesFails) {
  InferenceRulesBuilder builder;
  ir::Operator plus_operator("core.plus");
  ir::Operation plus(nullptr, plus_operator, {}, {});

  builder.MarkOperationAsAction(plus, "plus_action");
  EXPECT_DEATH(builder.MarkOperationAsAction(plus, "blah_action"),
               "Duplicate operation inserted for 'blah_action'");
}

TEST(InferenceRulesBuilderTest, CombiningAllBuildCommandsWork) {
  InferenceRulesBuilder builder;
  TagId tag_a = builder.GetOrCreateTagId("A");
  TagId tag_b = builder.GetOrCreateTagId("B");
  builder.AddOutputRulesForAction("action1", test_action_rules[0]);
  builder.AddOutputRulesForAction("action2", test_action_rules[1]);

  ir::Operator plus_operator("core.plus");
  ir::Operator minus_operator("core.minus");
  ir::Operation plus(nullptr, plus_operator, {}, {});
  ir::Operation minus(nullptr, minus_operator, {}, {});

  builder.MarkOperationAsAction(plus, "plus_action");
  builder.MarkOperationAsAction(minus, "minus_action");

  InferenceRules rules = builder.Build();
  EXPECT_THAT(
      rules.action_rules(),
      UnorderedElementsAre(std::make_pair("action1", test_action_rules[0]),
                           std::make_pair("action2", test_action_rules[1])));
  EXPECT_THAT(rules.actions(),
              UnorderedElementsAre(std::make_pair(&plus, "plus_action"),
                                   std::make_pair(&minus, "minus_action")));
  EXPECT_THAT(rules.tag_names(), UnorderedElementsAre("A", "B"));
  EXPECT_THAT(rules.tags(), UnorderedElementsAre(std::make_pair("A", tag_a),
                                                 std::make_pair("B", tag_b)));
}

}  // namespace
}  // namespace raksha::analysis::taint
