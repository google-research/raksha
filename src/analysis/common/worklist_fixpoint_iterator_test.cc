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
#include "src/analysis/common/worklist_fixpoint_iterator.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"

namespace raksha::analysis::common {
namespace {

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

// A simple graph of int nodes to use in tests.
class IntGraph {
 public:
  using Node = int;
  using NodeSet = absl::flat_hash_set<Node>;
  using Edge = std::pair<Node, Node>;
  using EdgeSet = absl::flat_hash_set<Edge>;
  using AdjacencyList = absl::flat_hash_map<Node, NodeSet>;

  IntGraph(AdjacencyList nodes) : nodes_(std::move(nodes)) {}

  const Node& GetEdgeSource(const Edge& edge) const { return edge.first; }
  const Node& GetEdgeTarget(const Edge& edge) const { return edge.second; }

  EdgeSet GetOutEdges(const Node& node) const {
    EdgeSet result;
    auto find_result = nodes_.find(node);
    CHECK(find_result != nodes_.end());
    for (const Node& tgt : find_result->second) {
      result.insert(std::make_pair(node, tgt));
    }
    return result;
  }

 private:
  AdjacencyList nodes_;
};

// Interpreter for computing reaching nodes and edges.
class ReachingNodesAndEdgesInterpreter {
 public:
  using Graph = IntGraph;
  using AbstractState = absl::flat_hash_set<IntGraph::Node>;

  ReachingNodesAndEdgesInterpreter(const IntGraph& graph) : graph_(&graph) {}

  IntGraph::NodeSet GetEntryNodes() const { return {1}; }

  AbstractState ApplyNodeTransformer(const IntGraph::Node& node,
                                     const AbstractState& in_state) const {
    AbstractState result = in_state;
    result.insert(node);
    return result;
  }

  AbstractState ApplyEdgeTransformer(const IntGraph::Edge& edge,
                                     const AbstractState& in_state) const {
    AbstractState result = in_state;
    // Encode an edge as source * 1000 + target.
    result.insert(graph_->GetEdgeSource(edge) * 1000 +
                  graph_->GetEdgeTarget(edge));
    return result;
  }

  bool AreStatesEqual(const AbstractState& lhs, const AbstractState& rhs) {
    return lhs == rhs;
  }

  AbstractState CombineStates(const AbstractState& lhs,
                              const AbstractState& rhs) {
    IntGraph::NodeSet result;
    absl::c_merge(lhs, rhs, std::inserter(result, result.begin()));
    return result;
  }

  AbstractState GetInitialState(const IntGraph::Node& node) const {
    return {node};
  }

 private:
  const IntGraph* graph_;
};

using ReachingNodesAndEdgesAnalysis =
    WorklistFixpointIterator<ReachingNodesAndEdgesInterpreter>;

TEST(WorklistFixpointIteratorTest, ComputesFixpoint) {
  ReachingNodesAndEdgesAnalysis solver;
  IntGraph g({{1, {2}},
              {2, {3}},
              {3, {4, 7}},
              {4, {5}},
              {5, {6}},
              {6, {3}},
              {7, {8, 9}},
              {8, {10}},
              {9, {}},
              {10, {}}});
  auto result = solver.ComputeFixpoint(g);

  IntGraph::NodeSet stateInLoop(
      {1, 2, 3, 4, 5, 6, /*edges*/ 1002, 2003, 3004, 4005, 5006, 6003});
  EXPECT_THAT(
      result,
      UnorderedElementsAre(
          Pair(1, IntGraph::NodeSet({1})),
          Pair(2, IntGraph::NodeSet({1, 2, /*edges*/ 1002})),
          Pair(3, stateInLoop), Pair(4, stateInLoop), Pair(5, stateInLoop),
          Pair(6, stateInLoop),
          Pair(7, IntGraph::NodeSet({1, 2, 3, 4, 5, 6, 7, /*edges*/ 1002, 2003,
                                     3004, 3007, 4005, 5006, 6003})),
          Pair(8,
               IntGraph::NodeSet({1, 2, 3, 4, 5, 6, 7, 8, /*edges*/ 1002, 2003,
                                  3004, 3007, 4005, 5006, 6003, 7008})),
          Pair(9,
               IntGraph::NodeSet({1, 2, 3, 4, 5, 6, 7, 9, /*edges*/ 1002, 2003,
                                  3004, 3007, 4005, 5006, 6003, 7009})),
          Pair(10, IntGraph::NodeSet({1, 2, 3, 4, 5, 6, 7, 8, 10,
                                      /*edges*/ 1002, 2003, 3004, 3007, 4005,
                                      5006, 6003, 7008, 8010}))));
}

}  // namespace
}  // namespace raksha::analysis::common
