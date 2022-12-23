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
#ifndef SRC_ANALYSIS_COMMON_MODULE_GRAPH_H_
#define SRC_ANALYSIS_COMMON_MODULE_GRAPH_H_

#include <cstddef>
#include <iterator>
#include <tuple>
#include <variant>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "absl/log/die_if_null.h"
#include "src/common/utils/iterator_adapter.h"
#include "src/common/utils/ranges.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::analysis::common {

class ModuleGraph {
 public:
  using EdgeIndex = size_t;
  using Node = std::variant<ir::Value, const ir::Operation*>;
  using IndexedNodeSet = absl::flat_hash_set<std::pair<EdgeIndex, Node>>;
  struct Edge {
    Node source;
    EdgeIndex index;
    Node target;
  };
  using AdjacencyList = absl::flat_hash_map<Node, IndexedNodeSet>;

  // Returns an edge constructed from the given source and indexed target.
  struct EdgeMaterializer {
    Edge operator()(const std::pair<EdgeIndex, Node>& target,
                    const Node& source) {
      return {.source = source, .index = target.first, .target = target.second};
    }
  };

  template <typename T>
  struct TargetMaterializer {
    std::pair<EdgeIndex, T> operator()(
        const std::pair<EdgeIndex, Node>& target) {
      CHECK(std::holds_alternative<T>(target.second))
          << "ModuleGraph has an unexpected edge.";
      return std::make_pair(target.first, std::get<T>(target.second));
    }
  };

  explicit ModuleGraph(const ir::Module* module);

  // Returns all the nodes in the graph.
  auto GetNodes() const { return utils::ranges::keys(adjacency_list_); }

  // Returns the out edges of the given source.
  auto GetOutEdges(const Node& source) const {
    return utils::make_adapted_range<EdgeMaterializer>(GetAdjacentNodes(source),
                                                       source);
  }

  // Returns the uses of the given value as (index, Operation) pairs.
  auto GetUses(const ir::Value& value) const {
    return utils::make_adapted_range<TargetMaterializer<const ir::Operation*>>(
        GetAdjacentNodes(value));
  }

  // Returns the results of the given operation as (index, Value) pairs.
  auto GetResults(const ir::Operation* operation) const {
    return utils::make_adapted_range<TargetMaterializer<ir::Value>>(
        GetAdjacentNodes(operation));
  }

  // Returns the source node of an edge.
  const Node& GetEdgeSource(const Edge& edge) const { return edge.source; }

  // Returns the target node of an edge.
  const Node& GetEdgeTarget(const Edge& edge) const { return edge.target; }

 private:
  const AdjacencyList::mapped_type& GetAdjacentNodes(const Node& source) const {
    auto find_result = adjacency_list_.find(source);
    CHECK(find_result != adjacency_list_.end())
        << "GetOutEdges: source not found in graph.";
    return find_result->second;
  }

  const ir::Module& module_;
  AdjacencyList adjacency_list_;
};

}  // namespace raksha::analysis::common

#endif  // SRC_ANALYSIS_COMMON_MODULE_GRAPH_H_
