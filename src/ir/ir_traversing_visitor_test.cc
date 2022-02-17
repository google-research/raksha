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
#include "src/ir/ir_traversing_visitor.h"

#include <memory>

#include "src/common/testing/gtest.h"
#include "src/ir/block_builder.h"
#include "src/ir/module.h"

namespace raksha::ir {
namespace {

// Just a small test visitor that collects the nodes as they are visited to make
// sure that  Pre and Post visits happened correctly.
class CollectingVisitor : public IRTraversingVisitor<CollectingVisitor> {
 public:
  enum class TraversalType { PRE = 0x1, POST = 0x2, BOTH = 0x3 };
  CollectingVisitor(TraversalType traversal_type)
      : pre_visits_(traversal_type == TraversalType::PRE ||
                    traversal_type == TraversalType::BOTH),
        post_visits_(traversal_type == TraversalType::POST ||
                     traversal_type == TraversalType::BOTH) {}

  void PreVisit(const Module& module) override {
    if (pre_visits_) nodes_.push_back(std::addressof(module));
  }
  void PostVisit(const Module& module) override {
    if (post_visits_) nodes_.push_back(std::addressof(module));
  }

  void PreVisit(const Block& block) override {
    if (pre_visits_) nodes_.push_back(std::addressof(block));
  }

  void PostVisit(const Block& block) override {
    if (post_visits_) nodes_.push_back(std::addressof(block));
  }

  void PreVisit(const Operation& operation) override {
    if (pre_visits_) nodes_.push_back(std::addressof(operation));
  }

  void PostVisit(const Operation& operation) override {
    if (post_visits_) nodes_.push_back(std::addressof(operation));
  }

  const std::vector<const void*>& nodes() const { return nodes_; }

 private:
  bool pre_visits_;
  bool post_visits_;
  std::vector<const void*> nodes_;
};

TEST(IRTraversingVisitorTest, TraversesModuleAsExpected) {
  Module global_module;
  const Block* block1 =
      std::addressof(global_module.AddBlock(std::make_unique<Block>()));
  const Block* block2 =
      std::addressof(global_module.AddBlock(std::make_unique<Block>()));

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::PRE);
  global_module.Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(std::addressof(global_module), block1, block2));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::POST);
  global_module.Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(block1, block2, std::addressof(global_module)));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::BOTH);
  global_module.Accept(all_order_visitor);
  EXPECT_THAT(
      all_order_visitor.nodes(),
      testing::ElementsAre(std::addressof(global_module), block1, block1,
                           block2, block2, std::addressof(global_module)));
}

TEST(IRTraversingVisitorTest, TraversesBlockAsExpected) {
  auto plus_op = std::make_unique<Operator>("core.plus");
  auto minus_op = std::make_unique<Operator>("core.minus");
  BlockBuilder builder;
  const Operation* plus_op_instance =
      std::addressof(builder.AddOperation(*plus_op, {}, {}));
  const Operation* minus_op_instance =
      std::addressof(builder.AddOperation(*minus_op, {}, {}));
  auto block = builder.build();

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::PRE);
  block->Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(block.get(), plus_op_instance, minus_op_instance));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::POST);
  block->Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(plus_op_instance, minus_op_instance, block.get()));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::BOTH);
  block->Accept(all_order_visitor);
  EXPECT_THAT(
      all_order_visitor.nodes(),
      testing::ElementsAre(block.get(), plus_op_instance, plus_op_instance,
                           minus_op_instance, minus_op_instance, block.get()));
}

TEST(IRTraversingVisitorTest, TraversesOperationAsExpected) {
  auto plus_op = std::make_unique<Operator>("core.plus");
  auto minus_op = std::make_unique<Operator>("core.minus");
  BlockBuilder builder;

  Operation plus_op_instance(nullptr, *plus_op, {}, {},
                             std::make_unique<Module>());
  auto plus_op_module = plus_op_instance.impl_module();

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::PRE);
  plus_op_instance.Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::POST);
  plus_op_instance.Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(plus_op_module, std::addressof(plus_op_instance)));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::BOTH);
  plus_op_instance.Accept(all_order_visitor);
  EXPECT_THAT(
      all_order_visitor.nodes(),
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module,
                           plus_op_module, std::addressof(plus_op_instance)));
}

}  // namespace
}  // namespace raksha::ir
