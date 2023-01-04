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
//----------------------------------------------------------------------------
#include "src/analysis/common/module_fixpoint_iterator.h"

#include "absl/algorithm/container.h"
#include "absl/container/btree_set.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/fold.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value_string_converter.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::analysis::common {
namespace {

using ::testing::UnorderedPointwise;

// A simple abstract value to keep track of set of reachable values.
class AbstractReachingOperations {
 public:
  AbstractReachingOperations(absl::btree_set<std::string> operations)
      : operations_(std::move(operations)) {}

  // Returns the bottom abstract value.
  static AbstractReachingOperations Bottom() {
    return AbstractReachingOperations({});
  }

  // Returns true if this abstract value is bottom.
  bool IsBottom() const { return operations_.empty(); }

  // Returns true if other is equivalent to this object.
  bool IsEquivalentTo(const AbstractReachingOperations& other) const {
    return operations_ == other.operations_;
  }

  const absl::btree_set<std::string>& operations() const { return operations_; }

  void AddOperation(absl::string_view operation) {
    operations_.insert(std::string(operation));
  }

  AbstractReachingOperations Join(
      const AbstractReachingOperations& other) const {
    absl::btree_set<std::string> new_operations;
    absl::c_set_union(operations_, other.operations_,
                      std::inserter(new_operations, new_operations.begin()));
    return AbstractReachingOperations(new_operations);
  }

 private:
  absl::btree_set<std::string> operations_;
};

using raksha::parser::ir::IrProgramParserResult;
using raksha::parser::ir::ParseProgram;

// A simple semantics class to track reachable values at each value.
class ReachingOperationsSemantics {
 public:
  using AbstractState = AbstractReachingOperations;
  ReachingOperationsSemantics(ir::SsaNames* ssa_names)
      : ssa_names_(ABSL_DIE_IF_NULL(ssa_names)) {}

  std::vector<AbstractState> ApplyOperationTransformer(
      const ir::Operation& operation,
      const std::vector<AbstractState>& input_states) const {
    // Compute join of reachable operations for each input.
    auto result = raksha::common::utils::fold(
        input_states, AbstractState({}),
        [](AbstractState so_far, const AbstractState& current) {
          return so_far.Join(current);
        });

    // Add the inputs to the reachable operations for the result.
    for (const auto& input : operation.inputs()) {
      result.AddOperation(ir::ValueToString(input, *ssa_names_));
    }
    return std::vector<AbstractState>(operation.NumberOfOutputs(), result);
  }

  AbstractState GetInitialState(const ir::Value& value) {
    // Returns the value prefixed by "Initial_".
    return AbstractState(
        {absl::StrCat("Initial_", ir::ValueToString(value, *ssa_names_))});
  }

 private:
  ir::SsaNames* ssa_names_;
};

using AbstractState = typename ReachingOperationsSemantics::AbstractState;
using ReachingOperationsAnalysis =
    ModuleFixpointIterator<ReachingOperationsSemantics>;

// A test case that contains module text and expected operations.
struct ModuleTestCase {
  std::string test_name;
  std::string module_text;
  absl::flat_hash_map<std::string, absl::btree_set<std::string>>
      expected_operations;
};

class ModuleFixpointIteratorTest
    : public ::testing::TestWithParam<ModuleTestCase> {};

// A matcher to compare computed fixpoint against the expected result.
//    fixpoint: std::pair<ir::Value, AbstractState>
//    expected: std::pair<string, btree_set<string>>
MATCHER_P(ValueFixpointEq, ssa_names, "") {
  const auto& [fixpoint, expected] = arg;
  const auto& [fixpoint_value, abstract_state] = fixpoint;
  const auto& [expected_value_string, expected_reachable] = expected;
  return ir::ValueToString(fixpoint_value, *ssa_names) ==
             expected_value_string &&
         abstract_state.operations() == expected_reachable;
}

TEST_P(ModuleFixpointIteratorTest, ComputesFixpoint) {
  ReachingOperationsAnalysis solver;
  const auto& [_test_name, module_text, expected_operations] = GetParam();
  IrProgramParserResult parse_result = ParseProgram(module_text);
  const ir::Module& module = *ABSL_DIE_IF_NULL(parse_result.module);
  ir::SsaNames* ssa_names = ABSL_DIE_IF_NULL(parse_result.ssa_names.get());
  auto result = solver.ComputeFixpoint(
      module,
      /*semantics_maker=*/[ssa_names](const ir::Module& module) {
        return ReachingOperationsSemantics(ssa_names);
      });

  EXPECT_THAT(result, UnorderedPointwise(ValueFixpointEq(ssa_names),
                                         expected_operations));
}

INSTANTIATE_TEST_SUITE_P(
    ModuleFixpointIteratorTests, ModuleFixpointIteratorTest,
    testing::ValuesIn<ModuleTestCase>(
        {{.test_name = "StraightLineSequenceConstructedProperly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0 = core.constant []()
                %1 = core.copy [](%0)
                %2 = core.copy [](%1)
                %3 = core.copy [](%2)
              }
            })",
          .expected_operations = {{"%0", {}},
                                  {"%1", {"%0"}},
                                  {"%2", {"%0", "%1"}},
                                  {"%3", {"%0", "%1", "%2"}}}},
         {.test_name = "MultipleInputsIsHandledCorrectly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0 = core.constant []()
                %1 = core.constant []()
                %2 = core.constant []()
                %3 = core.triple [](%0, %1, %2)
              }
            })",
          .expected_operations =
              {{"%0", {}}, {"%1", {}}, {"%2", {}}, {"%3", {"%0", "%1", "%2"}}}},
         {.test_name = "MultipleResultsIsHandledCorrectly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0, %1, %2 = core.triple []()
                %3 = core.output [](%0, %1, %2)
              }
            })",
          .expected_operations =
              {{"%0", {}}, {"%1", {}}, {"%2", {}}, {"%3", {"%0", "%1", "%2"}}}},
         {.test_name = "InitialValuesIsHandledCorrectly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0 = core.constant [](<<ANY>>)
                %1 = core.constant []()
                %2 = core.output [](%0, %1)
              }
            })",
          .expected_operations =
              {{"<<ANY>>", {"Initial_<<ANY>>"}},
               {"%0", {"<<ANY>>", "Initial_<<ANY>>"}},
               {"%1", {}},
               {"%2", {"<<ANY>>", "Initial_<<ANY>>", "%0", "%1"}}}},
         {.test_name = "LoopsAreHandledCorrectly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0 = core.input []()
                %1, %4 = core.choice [](%0, %3)
                %2 = core.copy [](%1)
                %3 = core.copy [](%2)
                %5 = core.copy [](%4)
              }
            })",
          .expected_operations = {{"%0", {}},
                                  {"%1", {"%0", "%1", "%2", "%3"}},
                                  {"%2", {"%0", "%1", "%2", "%3"}},
                                  {"%3", {"%0", "%1", "%2", "%3"}},
                                  {"%4", {"%0", "%1", "%2", "%3"}},
                                  {"%5", {"%0", "%1", "%2", "%3", "%4"}}}},
         {.test_name = "ComplexGraphIsHandledCorrectly",
          .module_text = R"(
            module m0 {
              block b0 {
                %0 = core.int_constant [value: 2]()
                %1 = core.string_constant [value: "hello"]()
                %2 = core.choose [](%0, %1)
                %3 = core.transform [](%2)
                %4, %5, %6 = core.split [](%3)
                %7 = core.pair [](%4, %5)
                %8 = core.copy [](%6)
                %9 = core.triple [](%7, %8, %6)
                %10 = core.output [](%9)
              }
            })",
          .expected_operations =
              {{"%0", {}},
               {"%1", {}},
               {"%2", {"%0", "%1"}},
               {"%3", {"%0", "%1", "%2"}},
               {"%4", {"%0", "%1", "%2", "%3"}},
               {"%5", {"%0", "%1", "%2", "%3"}},
               {"%6", {"%0", "%1", "%2", "%3"}},
               {"%7", {"%0", "%1", "%2", "%3", "%4", "%5"}},
               {"%8", {"%0", "%1", "%2", "%3", "%6"}},
               {"%9", {"%0", "%1", "%2", "%3", "%4", "%5", "%6", "%7", "%8"}},
               {"%10",
                {"%0", "%1", "%2", "%3", "%4", "%5", "%6", "%7", "%8",
                 "%9"}}}}}),
    [](const testing::TestParamInfo<ModuleFixpointIteratorTest::ParamType>&
           info) { return info.param.test_name; });

}  // namespace
}  // namespace raksha::analysis::common
