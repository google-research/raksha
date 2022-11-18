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
#ifndef SRC_ANALYSIS_COMMON_WORKLIST_FIXPOINT_ITERATOR_H_
#define SRC_ANALYSIS_COMMON_WORKLIST_FIXPOINT_ITERATOR_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"

namespace raksha::analysis::common {

template <typename AbstractInterpreter>
class WorklistFixpointIterator {
 public:
  // TODO(b/258095882): Add a way to encode requirements for AbstractInterpreter
  using Graph = typename AbstractInterpreter::Graph;
  using Node = typename Graph::Node;
  using Edge = typename Graph::Edge;
  using AbstractState = typename AbstractInterpreter::AbstractState;
  using NodeStateMap = absl::flat_hash_map<Node, AbstractState>;

  // Computes a fixpoint for the given graph and returns the computed fixpoint.
  NodeStateMap ComputeFixpoint(const Graph& graph) const {
    AbstractInterpreter interpreter(graph);
    NodeStateMap node_states;
    absl::flat_hash_set<Node> worklist;
    for (const Node& entry : interpreter.GetEntryNodes()) {
      worklist.insert(entry);
      node_states[entry] = interpreter.GetInitialState(entry);
    }

    while (!worklist.empty()) {
      // Select and remove a node from the worklist.
      Node node = *worklist.begin();
      worklist.erase(worklist.begin());

      // Apply node transformation.
      auto node_state_find_result = node_states.find(node);
      CHECK(node_state_find_result != node_states.end())
          << "Node in worklist without a corresponding state.";
      const AbstractState& node_state_in = node_state_find_result->second;

      AbstractState node_state_out =
          interpreter.ApplyNodeTransformer(node, node_state_in);

      // Apply edge transformation and propagate to targets of node.
      for (const Edge& edge : graph.GetOutEdges(node)) {
        AbstractState edge_out =
            interpreter.ApplyEdgeTransformer(edge, node_state_out);

        const Node& target = graph.GetEdgeTarget(edge);
        auto find_result = node_states.find(target);
        bool first_visit = find_result == node_states.end();
        const AbstractState& target_old_state_in =
            (first_visit) ? interpreter.GetInitialState(target)
                          : find_result->second;

        const AbstractState& target_new_state_in =
            interpreter.CombineStates(edge_out, target_old_state_in);

        if (first_visit || !interpreter.AreStatesEqual(target_new_state_in,
                                                       target_old_state_in)) {
          node_states.insert_or_assign(target, target_new_state_in);
          worklist.insert(target);
        }
      }
    }

    return node_states;
  }
};

}  // namespace raksha::analysis::common

#endif  // SRC_ANALYSIS_COMMON_WORKLIST_FIXPOINT_ITERATOR_H_
