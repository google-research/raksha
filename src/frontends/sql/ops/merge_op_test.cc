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
#include "src/frontends/sql/ops/merge_op.h"

#include <algorithm>

#include "src/common/testing/gtest.h"
#include "src/common/utils/ranges.h"
#include "src/frontends/sql/ops/example_value_test_helper.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {
namespace {

using ::testing::Combine;
using ::testing::ElementsAreArray;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::Values;
using ::testing::ValuesIn;

class MergeOpTest : public ::testing::TestWithParam<
                        std::tuple<ir::ValueList, ir::ValueList>> {
 public:
  MergeOpTest() { SqlOp::RegisterOperator<MergeOp>(context_); }

 protected:
  ir::IRContext context_;
};

TEST_P(MergeOpTest, FactoryMethodCreatesCorrectOperation) {
  const auto& [inputs, controls] = GetParam();
  // It is rare for the assignment to lose precision.
  // Even if it does, the test will most certainly fail.
  int64_t control_start_index = inputs.size();

  std::unique_ptr<ir::Operation> op =
      MergeOp::Create(nullptr, context_, inputs, controls);
  EXPECT_EQ(op->op().name(), OpTraits<MergeOp>::kName);
  EXPECT_EQ(op->parent(), nullptr);
  EXPECT_THAT(utils::make_range(op->inputs().begin(),
                                op->inputs().begin() + control_start_index),
              ElementsAreArray(inputs));
  EXPECT_THAT(utils::make_range(op->inputs().begin() + control_start_index,
                                op->inputs().end()),
              ElementsAreArray(controls));
  EXPECT_THAT(op->attributes(),
              UnorderedElementsAre(Pair(
                  std::string(MergeOp::kControlInputStartIndex),
                  ir::Attribute::Create<ir::Int64Attribute>(inputs.size()))));
}

TEST_P(MergeOpTest, EnsureInputsAndEmptyInParamAreNotEmptySimultaneously) {
  const auto& [inputs, controls] = GetParam();
  // This will cause failures which is tested in a separate death test.
  EXPECT_TRUE(!inputs.empty() || !controls.empty());
}

TEST_P(MergeOpTest, GetControlInputsReturnsCorrectValue) {
  const auto& [inputs, controls] = GetParam();
  std::unique_ptr<MergeOp> merge_op =
      MergeOp::Create(nullptr, context_, inputs, controls);
  EXPECT_THAT(merge_op->GetControlInputs(), ElementsAreArray(controls));
}

TEST_P(MergeOpTest, GetDirectInputsReturnsCorrectValue) {
  const auto& [inputs, controls] = GetParam();
  std::unique_ptr<MergeOp> merge_op =
      MergeOp::Create(nullptr, context_, inputs, controls);
  EXPECT_THAT(merge_op->GetDirectInputs(), ElementsAreArray(inputs));
}

TEST_P(MergeOpTest, GetControlInputStartIndexReturnsCorrectValue) {
  const auto& [inputs, controls] = GetParam();
  std::unique_ptr<MergeOp> merge_op =
      MergeOp::Create(nullptr, context_, inputs, controls);
  EXPECT_EQ(merge_op->GetControlInputStartIndex(), inputs.size());
}

// Death tests

TEST(MergeOpDeathTest, FailsIfMergeOpIsNotRegistered) {
  ir::IRContext context;
  EXPECT_DEATH(
      { MergeOp::Create(nullptr, context, {ir::Value(ir::value::Any())}, {}); },
      "GetOperator<MergeOp>.*Must be non (null|NULL)");
}

TEST(MergeOpDeathTest, FailsIfBothDirectAndControlInputsAreEmpty) {
  ir::IRContext context;
  SqlOp::RegisterOperator<MergeOp>(context);
  EXPECT_DEATH({ MergeOp::Create(nullptr, context, {}, {}); },
               "Both direct inputs and control inputs are empty.");
}

testing::ExampleValueTestHelper example_helper;

static const ir::ValueList kValueListExamples[] = {
    {example_helper.GetAny()},
    {example_helper.GetOperationResult("out"),
     example_helper.GetBlockArgument("arg"), example_helper.GetAny()}};

INSTANTIATE_TEST_SUITE_P(MultipleValueCombinations, MergeOpTest,
                         Combine(ValuesIn(kValueListExamples),
                                 ValuesIn(kValueListExamples)));
INSTANTIATE_TEST_SUITE_P(EmptyDirectInputs, MergeOpTest,
                         Combine(Values(ir::ValueList({})),
                                 ValuesIn(kValueListExamples)));
INSTANTIATE_TEST_SUITE_P(EmptyControlInputs, MergeOpTest,
                         Combine(ValuesIn(kValueListExamples),
                                 Values(ir::ValueList({}))));
}  // namespace
}  // namespace raksha::frontends::sql
