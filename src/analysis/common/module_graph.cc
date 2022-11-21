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
#include "src/analysis/common/module_graph.h"

#include "absl/strings/string_view.h"
#include "src/ir/ir_traversing_visitor.h"
#include "src/ir/value.h"

namespace raksha::analysis::common {

namespace {

class AdjacencyListComputingVisitor
    : public ir::IRTraversingVisitor<AdjacencyListComputingVisitor> {
 public:
  static ModuleGraph::AdjacencyList ComputeAdjacencyList(
      const ir::Module& module) {
    AdjacencyListComputingVisitor visitor;
    module.Accept(visitor);
    return std::move(visitor.adjacency_list_);
  }

 private:
  Unit PreVisit(const ir::Operation& operation) override {
    // Add an entry in the adjacency list for the operation.
    adjacency_list_.insert({ModuleGraph::Node(&operation), {}});

    // Add an entry in the adjacency list for every input of the operation.
    size_t input_index = 0;
    // Add edge: input --index--> operation)
    for (const auto& input : operation.inputs()) {
      auto [input_entry, input_insert_result] =
          adjacency_list_.insert({ModuleGraph::Node(input), {}});
      input_entry->second.insert(
          std::make_pair(input_index, ModuleGraph::Node(&operation)));
      ++input_index;
    }

    // Add edge: operation --index--> output
    for (uint64_t output_index = 0; output_index < operation.NumberOfOutputs();
         ++output_index) {
      // Mark the output value as a node in the graph.
      auto output_node =
          ModuleGraph::Node(operation.GetOutputValue(output_index));
      adjacency_list_.insert({output_node, {}});
      // Add the edge.
      auto [output_entry, output_insert_result] =
          adjacency_list_.insert({ModuleGraph::Node(&operation), {}});
      output_entry->second.insert(std::make_pair(output_index, output_node));
    }
    return Unit();
  }

  ModuleGraph::AdjacencyList adjacency_list_;
};

}  // namespace

ModuleGraph::ModuleGraph(const ir::Module* module)
    : module_(*ABSL_DIE_IF_NULL(module)),
      adjacency_list_(
          AdjacencyListComputingVisitor::ComputeAdjacencyList(module_)) {}

}  // namespace raksha::analysis::common
