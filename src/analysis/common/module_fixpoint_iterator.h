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
#ifndef SRC_ANALYSIS_COMMON_MODULE_FIXPOINT_ITERATOR_H_
#define SRC_ANALYSIS_COMMON_MODULE_FIXPOINT_ITERATOR_H_

#include <iterator>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "src/analysis/common/module_graph.h"
#include "src/common/utils/map_iter.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::analysis::common {

template <typename AbstractSemantics>
class ModuleFixpointIterator {
 public:
  // TODO(b/258095882): Add a way to encode requirements for AbstractSemantics
  using AbstractState = typename AbstractSemantics::AbstractState;
  using ValueStateMap = absl::flat_hash_map<ir::Value, AbstractState>;

  // Computes a least fixpoint for the given graph and returns the result.
  template <typename SemanticsMaker>
  ValueStateMap ComputeFixpoint(const ir::Module& module,
                                SemanticsMaker semantics_maker) const {
    ModuleGraph module_graph(&module);
    AbstractSemantics semantics = semantics_maker(module);
    ValueStateMap value_states;
    // TODO(b/264415551): We should use a priority-based worklist
    absl::flat_hash_set<const ir::Operation*> worklist;

    // Initialize worklist and get values of entry nodes.
    for (const auto& node : module_graph.GetNodes()) {
      std::visit(
          utils::overloaded{
              [&value_states, &module_graph, &worklist,
               &semantics](const ir::Value& value) {
                // If this is an operation result, nothing needs to be done.
                // The result for the value will be computed during iteration.
                if (value.If<ir::value::OperationResult>() != nullptr) return;

                // If this is not an operation result, get the initial value.
                auto [_, success] = value_states.insert(
                    {value, semantics.GetInitialState(value)});
                CHECK(success)
                    << "Duplicate entry nodes during fixpoint iteration.";
                // Add all operations that use this value to the worklist.
                for (const auto [index, operation] :
                     module_graph.GetUses(value)) {
                  worklist.insert(operation);
                }
              },
              [&worklist](const ir::Operation* operation_node) {
                // If an operation has no inputs add it to the initial worklist.
                const ir::Operation& operation =
                    *ABSL_DIE_IF_NULL(operation_node);
                if (operation.inputs().empty()) worklist.insert(&operation);
              }},
          node);
    }
    // A local lambda to get the current state during fixpoint iteration.
    auto get_current_state = [&value_states](const ir::Value& value) {
      auto find_result = value_states.find(value);
      bool first_visit = (find_result == value_states.end());
      return std::make_pair(
          (first_visit ? AbstractState::Bottom() : find_result->second),
          first_visit);
    };

    while (!worklist.empty()) {
      // Select and remove a node from the worklist.
      const ir::Operation& operation = *ABSL_DIE_IF_NULL(*worklist.begin());
      worklist.erase(worklist.begin());

      // Prepare a vector of states corresponding to inputs of the operation.
      std::vector<AbstractState> operation_inputs =
          utils::MapIter<AbstractState>(
              operation.inputs(), [&get_current_state](const ir::Value& value) {
                return get_current_state(value).first;
              });

      // Apply abstract semantics and compute outputs.
      std::vector<AbstractState> operation_outputs =
          semantics.ApplyOperationTransformer(operation, operation_inputs);
      CHECK(operation_outputs.size() == operation.NumberOfOutputs())
          << "Transformer for an operation returns insufficient outputs.";

      // Push the computed outputs to the corresponding results.
      for (const auto& [index, target] : module_graph.GetResults(&operation)) {
        CHECK(index >= 0 && index < operation_outputs.size())
            << "Edge index in module graph is out of bounds.";
        const AbstractState& edge_out = operation_outputs[index];

        const auto& [target_old_state_in, first_visit] =
            get_current_state(target);
        const AbstractState& target_new_state_in =
            target_old_state_in.Join(edge_out);

        if (first_visit ||
            !target_new_state_in.IsEquivalentTo(target_old_state_in)) {
          value_states.insert_or_assign(target, target_new_state_in);
          // Add all operations that use this value to the worklist.
          for (const auto& [index, operation] : module_graph.GetUses(target)) {
            worklist.insert(operation);
          }
        }
      }
    }
    return value_states;
  }
};

}  // namespace raksha::analysis::common

#endif  // SRC_ANALYSIS_COMMON_MODULE_FIXPOINT_ITERATOR_H_
