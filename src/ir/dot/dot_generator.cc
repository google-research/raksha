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
#include "src/ir/dot/dot_generator.h"

#include "absl/container/btree_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_format.h"
#include "src/ir/dot/dot_generator_config.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ir_traversing_visitor.h"
#include "src/ir/module.h"

namespace raksha::ir::dot {

class DotGeneratorHelper : public IRTraversingVisitor<DotGeneratorHelper> {
 public:
  // Defining these types as std::string for now. If needed, we can define a
  // richer structure for `Node` and `Edge` later.
  using Edge = std::pair<std::string, std::string>;
  using Node = std::string;

  // Returns a dot representation of the module.
  static std::string ModuleAsDot(const Module& module,
                                 DotGeneratorConfig config) {
    DotGeneratorHelper visitor(std::move(config));
    module.Accept(visitor);
    return visitor.GetDotGraph();
  }

  std::monostate PreVisit(const Block& block) override;
  std::monostate PreVisit(const Operation& operation) override;

 private:
  DotGeneratorHelper(DotGeneratorConfig config) : config_(std::move(config)) {}

  // Returns the captured state as a dot graph string.
  std::string GetDotGraph();

  // Returns the dot representation of an op.
  std::string GetDotNode(const Operation& op);

  // Returns the dot node name for the given `operation`.
  std::string GetNodeName(const Operation& operation) {
    const Block* block = operation.parent();
    auto id = ssa_names_.GetOrCreateID(operation);
    return absl::StrFormat(R"("%s_%s")",
                           block == nullptr ? "g" : GetNodeName(*block), id);
  }

  // Returns the dot node name for the given `block`.
  std::string GetNodeName(const Block& block) {
    auto id = ssa_names_.GetOrCreateID(block);
    return absl::StrFormat(R"(%s)", id);
  }

  // Returns the dot node name for the given `storage`.
  absl::string_view GetNodeName(const Storage& storage) const {
    return storage.name();
  }

  // Associates a name with the given op and returns that name.
  void AddOperationNode(absl::string_view node_name, const Operation& op);

  // Associates a name with the given block and returns that name.
  void AddBlockNode(absl::string_view node_name, const Block& op);

  // Adds a result for the given node;
  void AddResult(const Operation& op, absl::string_view name);

  // Adds an edge from `src` -> `tgt`.
  void AddEdge(absl::string_view src, absl::string_view tgt) {
    edges_.insert(Edge(Node(src), Node(tgt)));
  }

  // Mapping from node name to blocks.
  absl::flat_hash_map<std::string, const Block*> blocks_;
  // Mapping from node name to operation.
  absl::flat_hash_map<std::string, const Operation*> operations_;
  // Results of an operation that are used somewhere. The signature of an
  // operator does not *yet* provide enough information about the number of
  // results. This map infers this indirectly by adding one whenever an
  // operation result is encountered during dot generation.
  absl::flat_hash_map<std::string, absl::btree_set<std::string>>
      operation_results_;
  absl::flat_hash_set<Edge> edges_;
  SsaNames ssa_names_;
  DotGeneratorConfig config_;
};

void DotGeneratorHelper::AddOperationNode(absl::string_view node_name,
                                          const Operation& op) {
  auto insert_result = operations_.insert({std::string(node_name), &op});
  CHECK(insert_result.second) << "Adding a duplicate node for an op";
}

void DotGeneratorHelper::AddBlockNode(absl::string_view node_name,
                                      const Block& op) {
  auto insert_result = blocks_.insert({std::string(node_name), &op});
  CHECK(insert_result.second) << "Adding a duplicate node for a block";
}

void DotGeneratorHelper::AddResult(const Operation& op,
                                   absl::string_view name) {
  std::string node_name = GetNodeName(op);
  auto find_result = operation_results_.find(node_name);
  if (find_result == operation_results_.end()) {
    operation_results_.insert({node_name, {std::string(name)}});
  } else {
    find_result->second.insert(std::string(name));
  }
}

namespace {

// Safe initialization of static empty btree set.
const absl::btree_set<std::string>& GetEmptyStringBtreeSet() {
  static const absl::btree_set<std::string>* empty_set =
      new absl::btree_set<std::string>();
  return *empty_set;
}

}  // namespace

std::string DotGeneratorHelper::GetDotNode(const Operation& op) {
  // Generates a dot representation of an operation as a node. Suppose that the
  // operation has `m` inputs and `n`results, the generated node will be
  // rendered as follows:
  //
  //  +------------+
  //  |I0|I1|...|Im|
  //  +--+------+--+
  //  |   op info  |
  //  +--+--+---+--+
  //  |R0|R1|...|Rn|
  //  +--+--+---+--+
  //
  // The `Ix` and `Rx` are ports that we can connect edges to.
  // (cf. https://www.graphviz.org/doc/info/shapes.html#record)
  std::string node_name = GetNodeName(op);
  auto find_result = operation_results_.find(node_name);
  const absl::btree_set<std::string>& results =
      (find_result == operation_results_.end()) ? GetEmptyStringBtreeSet()
                                                : find_result->second;
  // The total_colspan is the LCM of the inputs and outputs, and will be used to
  // determine the colspan for the table cells in the rendered table.
  size_t input_cols = std::max(1uL, op.inputs().size());
  size_t output_cols = std::max(1uL, (size_t)results.size());
  size_t total_colspan = std::lcm(input_cols, output_cols);
  int input_colspan = total_colspan / input_cols;
  int output_colspan = total_colspan / output_cols;

  std::string input_ports =
      op.inputs().empty()
          ? absl::StrFormat(R"(<TD COLSPAN="%d">&nbsp;</TD>)", total_colspan)
          : absl::StrJoin(
                op.inputs(), "",
                [i = 0, input_colspan](std::string* out,
                                       const Value& v) mutable {
                  int node_num = i++;
                  absl::StrAppend(
                      out,
                      absl::StrFormat(R"(<TD COLSPAN="%d" PORT="I%d">I%d</TD>)",
                                      input_colspan, node_num, node_num));
                });
  std::string output_ports =
      results.empty()
          ? absl::StrFormat(R"(<TD COLSPAN="%d" PORT="out">out</TD>)",
                            total_colspan)
          : absl::StrJoin(
                results, "",
                [output_colspan](std::string* out, const std::string& name) {
                  absl::StrAppend(
                      out,
                      absl::StrFormat(R"(<TD COLSPAN="%d" PORT="%s">%s</TD>)",
                                      output_colspan, name, name));
                });
  std::string additional_label = config_.operation_labeler(op, results);
  std::string additional_label_directive =
      additional_label.empty()
          ? ""
          : absl::StrFormat(R"(<TR><TD COLSPAN="%d">%s </TD></TR>)",
                            total_colspan, additional_label);
  std::string attributes =
      op.attributes().empty()
          ? ""
          : absl::StrFormat(
                " [%s] ", IRPrinter::PrintNamedMapInNameOrder(
                              op.attributes(), [](const Attribute& attribute) {
                                return attribute.ToString();
                              }));
  return absl::StrFormat(
      R"(%s [label=<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0"><TR>%s</TR><TR><TD COLSPAN="%d">%s%s</TD></TR>%s<TR>%s</TR></TABLE>>])",
      std::move(node_name), std::move(input_ports), total_colspan,
      op.op().name(), std::move(attributes),
      std::move(additional_label_directive), std::move(output_ports));
}

std::string DotGeneratorHelper::GetDotGraph() {
  constexpr absl::string_view kDigraphFormat = R"(digraph G {
  node[shape=none];
  // Nodes:
  %s
  %s
  // Edges:
  %s
  // End:
}
)";
  // Genearte nodes for blocks
  std::string blocks = absl::StrJoin(
      blocks_, "\n  ",
      [](std::string* out,
         absl::flat_hash_map<std::string, const Block*>::value_type
             node_entry) {
        absl::StrAppend(
            out, absl::StrFormat("%s [shape=Mrecord]", node_entry.first));
      });
  // Generate nodes for operations.
  std::string operations = absl::StrJoin(
      operations_, "\n  ",
      [this](std::string* out,
             absl::flat_hash_map<std::string, const Operation*>::value_type
                 node_entry) {
        absl::StrAppend(out, GetDotNode(*CHECK_NOTNULL(node_entry.second)));
      });
  // Generate edges.
  std::string edges =
      absl::StrJoin(edges_, "\n  ", absl::PairFormatter(" -> "));

  return absl::StrFormat(kDigraphFormat, blocks, operations, edges);
}

std::monostate DotGeneratorHelper::PreVisit(const Operation& operation) {
  std::string node_name = GetNodeName(operation);
  AddOperationNode(node_name, operation);
  unsigned input_index = 0;
  for (const Value& value : operation.inputs()) {
    std::string input_name =
        absl::StrFormat("%s:I%d", node_name, input_index++);
    if (const auto* block_arg = value.If<value::BlockArgument>()) {
      // BlockArgument rendered as "block:arg_name"
      const Block& block = block_arg->block();
      std::string block_arg_name =
          absl::StrFormat("%s:%s", GetNodeName(block), block_arg->name());
      AddEdge(block_arg_name, input_name);
    } else if (const auto* op_result = value.If<value::OperationResult>()) {
      // OperationResult rendered as "op:result_name"
      const Operation& other_op = op_result->operation();
      std::string op_result_name =
          absl::StrFormat("%s:%s", GetNodeName(other_op), op_result->name());
      AddEdge(op_result_name, input_name);
      AddResult(other_op, op_result->name());
    } else if (const auto* stored_value = value.If<value::StoredValue>()) {
      // StoredValue rendered as "storage-name"
      AddEdge(GetNodeName(stored_value->storage()), input_name);
    }
  }
  return {};
}

std::monostate DotGeneratorHelper::PreVisit(const Block& block) {
  std::string block_name = GetNodeName(block);
  AddBlockNode(block_name, block);
  return {};
}

std::string DotGenerator::ModuleAsDot(const Module& module,
                                      DotGeneratorConfig config) const {
  return DotGeneratorHelper::ModuleAsDot(module, std::move(config));
}

}  // namespace raksha::ir::dot
