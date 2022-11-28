#include "src/analysis/common/module_graph.h"

#include <string>
#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/iterator_adapter.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value.h"
#include "src/ir/value_string_converter.h"
#include "src/parser/ir/ir_parser.h"
#include "re2/re2.h"

namespace raksha::analysis::common {
namespace {

using NodeId = size_t;

struct ModuleGraphTestCase {
  using Edge = std::tuple<NodeId, NodeId, NodeId>;
  std::string test_name;
  absl::flat_hash_map<std::string, NodeId> node_string_ids;
  absl::flat_hash_set<NodeId> expected_nodes;
  absl::flat_hash_map<NodeId, std::vector<Edge>> expected_edges;
};

// A class to help with testing the ModuleGraph generation code.
//
// The constructor of this class reads the `node_string_ids` field of
// `ModuleGraphTestCase` and constructs a graph. The field `node_string_ids` is
// a map from string representations of graph nodes (i.e., IR Value/IR
// Operation) to an id such as the following:
//
//   {{"%0", 0}, {"%1", 1}, {"%2", 2},
//    {"%0 = core.constant []()\n", 3},
//    {"%1 = core.copy [](%0)\n", 4},
//    {"%2 = core.output [](%1)\n", 5}}
//
// NOTE: you will need to include `\n` at the end.
//
// With the information in `node_string_id`, the constructor does the following:
//
// Step 1: Builds the following IR text:
//  module m0 {
//    block b0 {
//      %0 = core.constant []()
//      %1 = core.copy [](%0)
//      %2 = core.output [](%1)
//    }  // block b0
//  }  // module m0
//
// Step 2: Parses the IR, builds the module graph, and constructs the following
// maps: `ModuleGraph::Node -> NodeId` and `NodeId -> ModuleGraph`.
//
// The maps allow the tests to refer to the nodes in terms of the passed in ids.
class ModuleGraphTest : public ::testing::TestWithParam<ModuleGraphTestCase> {
 public:
  ModuleGraphTest();
  const ModuleGraph& graph() const { return graph_; }

  // Returns the node ids in the graph.
  auto GetNodes() const {
    return utils::make_adapted_range<GraphNodeToIdMapper>(graph_.GetNodes(),
                                                          node_to_id_);
  }

  const ModuleGraph::Node& GetNode(NodeId id) const {
    return id_to_node_.at(id);
  }

  // Returns the out edges of the node with given `id`.
  auto GetOutEdges(NodeId id) const {
    return utils::make_adapted_range<GraphEdgeToIdMapper>(
        graph_.GetOutEdges(GetNode(id)), node_to_id_);
  }

 private:
  // Returns the selected node id for the given node.
  struct GraphNodeToIdMapper {
    NodeId operator()(
        const ModuleGraph::Node& node,
        const absl::flat_hash_map<ModuleGraph::Node, NodeId>& node_ids) const {
      auto find_result = node_ids.find(node);
      CHECK(find_result != node_ids.end()) << "Unable to find a node id";
      return find_result->second;
    }
  };

  // Replaces the ModuleGraph::Node entities in an ModuleGraph::Edge with the
  // corresponding id and returns the corresponding tuple.
  struct GraphEdgeToIdMapper {
    std::tuple<NodeId, ModuleGraph::EdgeIndex, NodeId> operator()(
        const ModuleGraph::Edge& edge,
        const absl::flat_hash_map<ModuleGraph::Node, NodeId>& node_ids) const {
      return std::make_tuple(GraphNodeToIdMapper()(edge.source, node_ids),
                             edge.index,
                             GraphNodeToIdMapper()(edge.target, node_ids));
    }
  };

  ir::IrProgramParser::Result parse_result_;
  ModuleGraph graph_;
  absl::flat_hash_map<ModuleGraph::Node, NodeId> node_to_id_;
  absl::flat_hash_map<NodeId, ModuleGraph::Node> id_to_node_;
};

TEST_P(ModuleGraphTest, GraphIsConstructedProperly) {
  const auto& param = GetParam();
  EXPECT_THAT(GetNodes(), ::testing::UnorderedPointwise(::testing::Eq(),
                                                        param.expected_nodes));
  for (const auto& node : GetNodes()) {
    auto edge_result = param.expected_edges.find(node);
    ASSERT_NE(edge_result, param.expected_edges.end())
        << "Unable to find expected edges for node: " << node;
    EXPECT_THAT(GetOutEdges(node), ::testing::UnorderedPointwise(
                                       ::testing::Eq(), edge_result->second));

    for (const auto& edge : graph().GetOutEdges(GetNode(node))) {
      const auto& [source, index, target] = edge;
      EXPECT_THAT(graph().GetEdgeSource(edge), source);
      EXPECT_THAT(graph().GetEdgeTarget(edge), target);
    }
  }
}

TEST_P(ModuleGraphTest, GetEdgeTargetAndGetEdgeSourceBehavesCorrectly) {
  for (const auto& node : GetNodes()) {
    for (const auto& edge : graph().GetOutEdges(GetNode(node))) {
      const auto& [source, index, target] = edge;
      EXPECT_THAT(graph().GetEdgeSource(edge), source);
      EXPECT_THAT(graph().GetEdgeTarget(edge), target);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    ModuleGraphTests, ModuleGraphTest,
    testing::ValuesIn<ModuleGraphTestCase>(
        {{.test_name = "StraightLineSequenceConstructedProperly",
          .node_string_ids = {{"%0", 0},
                              {"%1", 1},
                              {"%2", 2},
                              {"%3", 3},
                              {"%0 = core.constant []()\n", 4},
                              {"%1 = core.copy [](%0)\n", 5},
                              {"%2 = core.copy [](%1)\n", 6},
                              {"%3 = core.copy [](%2)\n", 7}},
          .expected_nodes = {0, 1, 2, 3, 4, 5, 6, 7},
          .expected_edges = {{0, {{0, 0, 5}}},
                             {1, {{1, 0, 6}}},
                             {2, {{2, 0, 7}}},
                             {3, {}},
                             {4, {{4, 0, 0}}},
                             {5, {{5, 0, 1}}},
                             {6, {{6, 0, 2}}},
                             {7, {{7, 0, 3}}}}},
         {.test_name = "IndexIsModeledCorrectlyForMultipleInputs",
          .node_string_ids = {{"%0", 0},
                              {"%1", 1},
                              {"%2", 2},
                              {"%3", 3},
                              {"%0 = core.constant []()\n", 4},
                              {"%1 = core.constant []()\n", 5},
                              {"%2 = core.constant []()\n", 6},
                              {"%3 = core.triple [](%0, %1, %2)\n", 7}},
          .expected_nodes = {0, 1, 2, 3, 4, 5, 6, 7},
          .expected_edges = {{0, {{0, 0, 7}}},
                             {1, {{1, 1, 7}}},
                             {2, {{2, 2, 7}}},
                             {3, {}},
                             {4, {{4, 0, 0}}},
                             {5, {{5, 0, 1}}},
                             {6, {{6, 0, 2}}},
                             {7, {{7, 0, 3}}}}},
         {.test_name = "IndexIsModeledCorrectlyForMultipleResults",
          .node_string_ids = {{"%0", 0},
                              {"%1", 1},
                              {"%2", 2},
                              {"%3", 3},
                              {"%0, %1, %2 = core.triple []()\n", 4},
                              {"%3 = core.output [](%0, %1, %2)\n", 5}},
          .expected_nodes = {0, 1, 2, 3, 4, 5},
          .expected_edges = {{0, {{0, 0, 5}}},
                             {1, {{1, 1, 5}}},
                             {2, {{2, 2, 5}}},
                             {3, {}},
                             {4, {{4, 0, 0}, {4, 1, 1}, {4, 2, 2}}},
                             {5, {{5, 0, 3}}}}},
         {.test_name = "LoopsAreHandledCorrectly",
          .node_string_ids = {{"%0", 0},
                              {"%1", 1},
                              {"%2", 2},
                              {"%3", 3},
                              {"%4", 4},
                              {"%5", 5},
                              {"%0 = core.input []()\n", 6},
                              {"%1, %4 = core.choice [](%0, %3)\n", 7},
                              {"%2 = core.copy [](%1)\n", 8},
                              {"%3 = core.copy [](%2)\n", 9},
                              {"%5 = core.copy [](%4)\n", 10}},
          .expected_nodes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
          .expected_edges = {{0, {{0, 0, 7}}},
                             {1, {{1, 0, 8}}},
                             {2, {{2, 0, 9}}},
                             {3, {{3, 1, 7}}},
                             {4, {{4, 0, 10}}},
                             {5, {}},
                             {6, {{6, 0, 0}}},
                             {7, {{7, 0, 1}, {7, 1, 4}}},
                             {8, {{8, 0, 2}}},
                             {9, {{9, 0, 3}}},
                             {10, {{10, 0, 5}}}}},
         {.test_name = "ComplexGraphIsHandledCorrectly",
          .node_string_ids =
              {{"%0", 0},
               {"%1", 1},
               {"%2", 2},
               {"%3", 3},
               {"%4", 4},
               {"%5", 5},
               {"%6", 6},
               {"%7", 7},
               {"%8", 8},
               {"%9", 9},
               {"%10", 10},
               {"%0 = core.int_constant [value: 2]()\n", 11},
               {"%1 = core.string_constant [value: \"hello\"]()\n", 12},
               {"%2 = core.choose [](%0, %1)\n", 13},
               {"%3 = core.transform [](%2)\n", 14},
               {"%4, %5, %6 = core.split [](%3)\n", 15},
               {"%7 = core.pair [](%4, %5)\n", 16},
               {"%8 = core.copy [](%6)\n", 17},
               {"%9 = core.triple [](%7, %8, %6)\n", 18},
               {"%10 = core.output [](%9)\n", 19}},
          .expected_nodes = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                             10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
          .expected_edges = {{0, {{0, 0, 13}}},
                             {1, {{1, 1, 13}}},
                             {2, {{2, 0, 14}}},
                             {3, {{3, 0, 15}}},
                             {4, {{4, 0, 16}}},
                             {5, {{5, 1, 16}}},
                             {6, {{6, 0, 17}, {6, 2, 18}}},
                             {7, {{7, 0, 18}}},
                             {8, {{8, 1, 18}}},
                             {9, {{9, 0, 19}}},
                             {10, {}},
                             {11, {{11, 0, 0}}},
                             {12, {{12, 0, 1}}},
                             {13, {{13, 0, 2}}},
                             {14, {{14, 0, 3}}},
                             {15, {{15, 0, 4}, {15, 1, 5}, {15, 2, 6}}},
                             {16, {{16, 0, 7}}},
                             {17, {{17, 0, 8}}},
                             {18, {{18, 0, 9}}},
                             {19, {{19, 0, 10}}}}}}),
    [](const testing::TestParamInfo<ModuleGraphTest::ParamType>& info) {
      return info.param.test_name;
    });

//----------------------------------------
// Implementation of the helper code

// Returns a pretty printed version of the given node.
std::string GetPrettyPrintedNode(const ModuleGraph::Node& node,
                                 ir::SsaNames& ssa_names) {
  if (auto operation = std::get_if<const ir::Operation*>(&node)) {
    return ir::IRPrinter::ToString(**operation, ssa_names);
  } else if (auto value = std::get_if<ir::Value>(&node)) {
    return ValueToString(*value, ssa_names);
  } else {
    return "<<UNKNOWN>>";
  }
}

// Builds an IR with the given nodes, parses it and returns the result.
ir::IrProgramParser::Result BuildIRAndParse(
    const absl::flat_hash_map<std::string, NodeId>& nodes) {
  // Concatenate all operations in the given nodes map and build the IR.
  auto ir = absl::Substitute(
      R"(
module m0 {
  block b0 {
    $0
  }  // block b0
}  // module m0
)",
      absl::StrJoin(utils::ranges::keys(nodes), "",
                    [](std::string* out, const std::string& value) {
                      // Filter values (like $0) and only include
                      // operations.
                      absl::StrAppend(
                          out, RE2::FullMatch(value, "%[0-9]+") ? "" : value);
                    }));
  // Parse the generated IR.
  return ir::IrProgramParser().ParseProgram(ir);
}

// Given a map from string representation of a node to an id, builds and
// returns a map from node to the corresponding id.
absl::flat_hash_map<ModuleGraph::Node, NodeId> BuildNodeToIdMap(
    const ModuleGraph& graph,
    const absl::flat_hash_map<std::string, NodeId>& nodes,
    ir::SsaNames& ssa_names) {
  absl::flat_hash_map<ModuleGraph::Node, NodeId> result;
  for (const auto& node : graph.GetNodes()) {
    std::string node_ir = GetPrettyPrintedNode(node, ssa_names);
    auto find_result = nodes.find(node_ir);
    CHECK(find_result != nodes.end())
        << "Could not find a node id for " << node_ir;
    const auto insert_result = result.insert({node, find_result->second});
    CHECK(insert_result.second) << "Duplicate node id";
  }
  return result;
}

absl::flat_hash_map<NodeId, ModuleGraph::Node> BuildIdToNodeMap(
    const absl::flat_hash_map<ModuleGraph::Node, NodeId>& node_to_id) {
  absl::flat_hash_map<NodeId, ModuleGraph::Node> ids_to_node_map;
  for (const auto& [node, id] : node_to_id) {
    auto insert_result = ids_to_node_map.insert({id, node});
    CHECK(insert_result.second) << "Duplicate node id";
  }
  return ids_to_node_map;
}

ModuleGraphTest::ModuleGraphTest()
    : parse_result_(BuildIRAndParse(GetParam().node_string_ids)),
      graph_(parse_result_.module.get()),
      node_to_id_(BuildNodeToIdMap(graph_, GetParam().node_string_ids,
                                   *parse_result_.ssa_names)),
      id_to_node_(BuildIdToNodeMap(node_to_id_)) {}

}  // namespace
}  // namespace raksha::analysis::common
