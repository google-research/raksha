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

#include "absl/strings/str_split.h"
#include "src/common/testing/gtest.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/ir_printer.h"
#include "src/ir/module.h"

namespace raksha::ir::dot {

namespace {

class DotGeneratorTest : public testing::Test {
 public:
  DotGeneratorTest()
      : pair_op_(&ir_context_.RegisterOperator(
            std::make_unique<Operator>("core.pair"))),
        plus_op_(&ir_context_.RegisterOperator(
            std::make_unique<Operator>("core.plus"))),
        minus_op_(&ir_context_.RegisterOperator(
            std::make_unique<Operator>("core.minus"))) {}

  const Operator& pair_op() const { return *pair_op_; }
  const Operator& plus_op() const { return *plus_op_; }
  const Operator& minus_op() const { return *minus_op_; }
  Module& global_module() { return global_module_; }

  // Renders the graph and extracts the returned string as vector of lines.
  // This method also trims the leading and trailing whitespaces each line.
  std::vector<std::string> GetDotGraphAsWhitespaceTrimmedLines(
      const Module& module) const {
    DotGenerator dot_generator;
    std::string dot_graph = dot_generator.ModuleAsDot(module);
    LOG(WARNING) << dot_graph;
    std::vector<std::string> lines =
        absl::StrSplit(dot_graph, "\n", absl::SkipWhitespace());
    absl::c_for_each(lines, [](std::string& line) {
      return absl::StripLeadingAsciiWhitespace(&line);
    });
    absl::c_for_each(lines, [](std::string& line) {
      return absl::StripTrailingAsciiWhitespace(&line);
    });
    return lines;
  }

  utils::iterator_range<std::vector<std::string>::const_iterator> GetNodes(
      const std::vector<std::string>& dot_graph_lines) const {
    return utils::make_range(absl::c_find(dot_graph_lines, "// Nodes:") + 1,
                             absl::c_find(dot_graph_lines, "// Edges:"));
  }

  utils::iterator_range<std::vector<std::string>::const_iterator> GetEdges(
      const std::vector<std::string>& dot_graph_lines) const {
    return utils::make_range(absl::c_find(dot_graph_lines, "// Edges:") + 1,
                             absl::c_find(dot_graph_lines, "// End:"));
  }

 protected:
  IRContext ir_context_;
  Module global_module_;
  const Operator* pair_op_;
  const Operator* plus_op_;
  const Operator* minus_op_;
};

TEST_F(DotGeneratorTest, CreatesNodeForBlocks) {
  // module m0 {
  //   block b0 {
  //   }  // block b0
  //   block b1 {
  //   }  // block b1
  // }  // module m0
  global_module().AddBlock(BlockBuilder().build());
  global_module().AddBlock(BlockBuilder().build());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());
  EXPECT_THAT(GetNodes(dot_graph_lines),
              testing::UnorderedElementsAre("B0", "B1"));
}

TEST_F(DotGeneratorTest, CreatesNodesForOperations) {
  // module m0 {
  //   block b0 {
  //     %0 = core.plus []()
  //     %1 = core.minus []()
  //   }  // block b0
  // }  // module m0
  BlockBuilder builder;
  ir::Value any_value((value::Any()));
  builder.AddOperation(plus_op(), {}, {});
  builder.AddOperation(minus_op(), {}, {});
  global_module().AddBlock(builder.build());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());
  EXPECT_THAT(GetNodes(dot_graph_lines),
              testing::UnorderedElementsAre(
                  //   block b0
                  "B0",
                  // %0 = core.plus []()
                  R"(B0_0[label="{{}|core.plus|{<out>out}}"])",
                  // %1 = core.minus []()
                  R"(B0_1[label="{{}|core.minus|{<out>out}}"])"));
}

TEST_F(DotGeneratorTest, CreatesAttributesInNodesForOperation) {
  // module m0 {
  //   block b0 {
  //     %0 = core.plus [string:"something", ttl:10]()
  //   }  // block b0
  // }  // module m0
  BlockBuilder builder;
  builder.AddOperation(
      plus_op(),
      NamedAttributeMap(
          {{"string", Attribute::Create<StringAttribute>("something")},
           {"ttl", Attribute::Create<Int64Attribute>(10)}}),
      {});
  global_module().AddBlock(builder.build());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());
  EXPECT_THAT(
      GetNodes(dot_graph_lines),
      testing::UnorderedElementsAre(
          // block b0
          "B0",
          // %0 = core.plus [string:"something", ttl:10]()
          R"(B0_0[label="{{}|core.plus [string: something, ttl: 10]|{<out>out}}"])"));
}

TEST_F(DotGeneratorTest, CreatesInputPortsInNodesForOperation) {
  ir::Value any_value((value::Any()));
  // module m0 {
  //   block b0 {
  //     %0 = core.plus [](<<ANY>>, <<ANY>>)
  //     %1 = core.minus [](<<ANY>>)
  //   }  // block b0
  // }  // module m0
  BlockBuilder builder;
  builder.AddOperation(plus_op(), {}, {any_value, any_value});
  builder.AddOperation(minus_op(), {}, {any_value});
  global_module().AddBlock(builder.build());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());
  EXPECT_THAT(GetNodes(dot_graph_lines),
              testing::UnorderedElementsAre(
                  // block b0
                  "B0",
                  // %0 = core.plus [](<<ANY>>, <<ANY>>)
                  R"(B0_0[label="{{<I0>I0|<I1>I1}|core.plus|{<out>out}}"])",
                  // %1 = core.minus [](<<ANY>>)
                  R"(B0_1[label="{{<I0>I0}|core.minus|{<out>out}}"])"));
}

TEST_F(DotGeneratorTest, CreatesOutputPortsInNodesForOperation) {
  ir::Value any_value((value::Any()));
  // module m0 {
  //   block b0 {
  //     %0 = core.pair [](<<ANY>>, <<ANY>>)
  //     %1 = core.plus [](%0.first, %1.second)
  //   }  // block b0
  // }  // module m0
  BlockBuilder builder;
  const Operation& pair =
      builder.AddOperation(pair_op(), {}, {any_value, any_value});
  builder.AddOperation(plus_op(), {},
                       {Value(value::OperationResult(pair, "first")),
                        Value(value::OperationResult(pair, "second"))});
  global_module().AddBlock(builder.build());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());
  EXPECT_THAT(
      GetNodes(dot_graph_lines),
      testing::UnorderedElementsAre(
          // block b0
          "B0",
          // %0 = core.pair [](<<ANY>>, <<ANY>>)
          R"(B0_0[label="{{<I0>I0|<I1>I1}|core.pair|{<first>first|<second>second}}"])",
          // %1 = core.plus [](%0.first, %1.second)
          R"(B0_1[label="{{<I0>I0|<I1>I1}|core.plus|{<out>out}}"])"));
}

TEST_F(DotGeneratorTest, AddsEdgeFromOperationResultToInputs) {
  BlockBuilder builder;
  ir::Value any_value((value::Any()));
  // module m0 {
  //   block b0 {
  //     %0 = core.minus [](<<ANY>>)
  //     %1 = core.plus [](%0.out, <<ANY>>)
  //     %2 = core.plus [](%1.out, %0.out)
  //   }  // block b0
  // }  // module m0
  const Operation& minus = builder.AddOperation(minus_op(), {}, {any_value});
  const Operation& plus = builder.AddOperation(
      plus_op(), {}, {Value(value::OperationResult(minus, "out")), any_value});
  builder.AddOperation(plus_op(), {},
                       {Value(value::OperationResult(plus, "out")),
                        Value(value::OperationResult(minus, "out"))});
  global_module().AddBlock(builder.build());
  LOG(WARNING) << IRPrinter::ToString(global_module());
  const auto& dot_graph_lines =
      GetDotGraphAsWhitespaceTrimmedLines(global_module());

  EXPECT_THAT(GetNodes(dot_graph_lines),
              testing::UnorderedElementsAre(
                  // block b0
                  "B0",
                  // %0 = core.minus [](<<ANY>>)
                  R"(B0_0[label="{{<I0>I0}|core.minus|{<out>out}}"])",
                  // %1 = core.plus [](%0.out, <<ANY>>)
                  R"(B0_1[label="{{<I0>I0|<I1>I1}|core.plus|{<out>out}}"])",
                  // %2 = core.plus [](%1.out, %0.out)
                  R"(B0_2[label="{{<I0>I0|<I1>I1}|core.plus|{<out>out}}"])"));
  EXPECT_THAT(
      GetEdges(dot_graph_lines),
      testing::UnorderedElementsAre(
          "B0_0:out -> B0_1:I0", "B0_0:out -> B0_2:I1", "B0_1:out -> B0_2:I0"));
}

}  // namespace

}  // namespace raksha::ir::dot
