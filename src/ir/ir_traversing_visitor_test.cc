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
  enum class TraversalType { kPre = 0x1, kPost = 0x2, kBoth = 0x3 };
  CollectingVisitor(TraversalType traversal_type)
      : pre_visits_(traversal_type == TraversalType::kPre ||
                    traversal_type == TraversalType::kBoth),
        post_visits_(traversal_type == TraversalType::kPost ||
                     traversal_type == TraversalType::kBoth) {}

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

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::kPre);
  global_module.Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(std::addressof(global_module), block1, block2));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::kPost);
  global_module.Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(block1, block2, std::addressof(global_module)));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::kBoth);
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

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::kPre);
  block->Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(block.get(), plus_op_instance, minus_op_instance));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::kPost);
  block->Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(plus_op_instance, minus_op_instance, block.get()));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::kBoth);
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

  CollectingVisitor preorder_visitor(CollectingVisitor::TraversalType::kPre);
  plus_op_instance.Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module));

  CollectingVisitor postorder_visitor(CollectingVisitor::TraversalType::kPost);
  plus_op_instance.Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(plus_op_module, std::addressof(plus_op_instance)));

  CollectingVisitor all_order_visitor(CollectingVisitor::TraversalType::kBoth);
  plus_op_instance.Accept(all_order_visitor);
  EXPECT_THAT(
      all_order_visitor.nodes(),
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module,
                           plus_op_module, std::addressof(plus_op_instance)));
}
}  // namespace

namespace {
using ResultType=std::vector<void*>;

// Another small test visitor that collects the nodes as they are visited to make
// sure that  Pre and Post visits happened correctly.
// Uses 'returns' rather than side effects.
class ReturningVisitor : public IRTraversingVisitor<ReturningVisitor, ResultType> {
 public:
  enum class TraversalType { kPre = 0x1, kPost = 0x2, kBoth = 0x3 };
  ReturningVisitor(TraversalType traversal_type)
      : pre_visits_(traversal_type == TraversalType::kPre ||
                    traversal_type == TraversalType::kBoth),
        post_visits_(traversal_type == TraversalType::kPost ||
                     traversal_type == TraversalType::kBoth) {}

  ResultType ReturningPreVisit(const Module& module) override {
    ResultType result;
    if (pre_visits_) result.push_back((void*)std::addressof(module));
    return result;
  }
  ResultType FoldResult(const Module& module, ResultType child_result, ResultType result) override {
    result.insert( result.end(), child_result.begin(), child_result.end() );
    return result;
  }
  ResultType PostVisit(const Module& module, ResultType result) override {
    if (post_visits_) result.push_back((void*)std::addressof(module));
    return result;
  }

  ResultType ReturningPreVisit(const Block& block) override {
    ResultType result;
    if (pre_visits_) result.push_back((void*)std::addressof(block));
    return result;
  }

  ResultType FoldResult(const Block& block, ResultType child_result, ResultType result) override {
    result.insert( result.end(), child_result.begin(), child_result.end() );
    return result;
  }
  ResultType PostVisit(const Block& block, ResultType result) override {
    if (post_visits_) result.push_back((void*)std::addressof(block));
    return result;
  }

  ResultType ReturningPreVisit(const Operation& operation) override {
    ResultType result;
    if (pre_visits_) result.push_back((void*)std::addressof(operation));
    return result;
  }

  ResultType FoldResult(const Operation& operation, ResultType child_result, ResultType result) override {
    result.insert( result.end(), child_result.begin(), child_result.end() );
    return result;
  }
  ResultType PostVisit(const Operation& operation, ResultType result) override {
    if (post_visits_) result.push_back((void*)std::addressof(operation));
    return result;
  }

 private:
  const bool pre_visits_;
  const bool post_visits_;
};

TEST(IRTraversingVisitorTest, TraversesModuleAsExpectedUsingReturns) {
  Module global_module;
  const Block* block1 =
      std::addressof(global_module.AddBlock(std::make_unique<Block>()));
  const Block* block2 =
      std::addressof(global_module.AddBlock(std::make_unique<Block>()));

  ReturningVisitor preorder_visitor(ReturningVisitor::TraversalType::kPre);
  ResultType nodes1 = global_module.Accept<ReturningVisitor, ResultType>(preorder_visitor);
  EXPECT_THAT(
      nodes1,
      testing::ElementsAre(std::addressof(global_module), block1, block2));

  ReturningVisitor postorder_visitor(ReturningVisitor::TraversalType::kPost);
  ResultType nodes2 = global_module.Accept(postorder_visitor);
  EXPECT_THAT(
      nodes2,
      testing::ElementsAre(block1, block2, std::addressof(global_module)));

  ReturningVisitor all_order_visitor(ReturningVisitor::TraversalType::kBoth);
  ResultType nodes3 = global_module.Accept(all_order_visitor);
  EXPECT_THAT(
      nodes3,
      testing::ElementsAre(std::addressof(global_module), block1, block1,
                           block2, block2, std::addressof(global_module)));
}

TEST(IRTraversingVisitorTest, TraversesBlockAsExpectedUsingReturns) {
  auto plus_op = std::make_unique<Operator>("core.plus");
  auto minus_op = std::make_unique<Operator>("core.minus");
  BlockBuilder builder;
  const Operation* plus_op_instance =
      std::addressof(builder.AddOperation(*plus_op, {}, {}));
  const Operation* minus_op_instance =
      std::addressof(builder.AddOperation(*minus_op, {}, {}));
  auto block = builder.build();

  ReturningVisitor preorder_visitor(ReturningVisitor::TraversalType::kPre);
  ResultType nodes1 = block->Accept<ReturningVisitor, ResultType>(preorder_visitor);
  EXPECT_THAT(
      nodes1,
      testing::ElementsAre(block.get(), plus_op_instance, minus_op_instance));

  ReturningVisitor postorder_visitor(ReturningVisitor::TraversalType::kPost);
  ResultType nodes2 = block->Accept(postorder_visitor);
  EXPECT_THAT(
      nodes2,
      testing::ElementsAre(plus_op_instance, minus_op_instance, block.get()));

  ReturningVisitor all_order_visitor(ReturningVisitor::TraversalType::kBoth);
  ResultType nodes3 = block->Accept(all_order_visitor);
  EXPECT_THAT(
      nodes3,
      testing::ElementsAre(block.get(), plus_op_instance, plus_op_instance,
                           minus_op_instance, minus_op_instance, block.get()));
}

TEST(IRTraversingVisitorTest, TraversesOperationAsExpectedUsingReturns) {
  auto plus_op = std::make_unique<Operator>("core.plus");
  auto minus_op = std::make_unique<Operator>("core.minus");
  BlockBuilder builder;

  Operation plus_op_instance(nullptr, *plus_op, {}, {},
                             std::make_unique<Module>());
  auto plus_op_module = plus_op_instance.impl_module();

  ReturningVisitor preorder_visitor(ReturningVisitor::TraversalType::kPre);
  ResultType nodes1 = plus_op_instance.Accept(preorder_visitor);
  EXPECT_THAT(
      nodes1,
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module));

  ReturningVisitor postorder_visitor(ReturningVisitor::TraversalType::kPost);
  ResultType nodes2 = plus_op_instance.Accept(postorder_visitor);
  EXPECT_THAT(
      nodes2,
      testing::ElementsAre(plus_op_module, std::addressof(plus_op_instance)));

  ReturningVisitor all_order_visitor(ReturningVisitor::TraversalType::kBoth);
  ResultType nodes3 = plus_op_instance.Accept(all_order_visitor);
  EXPECT_THAT(
      nodes3,
      testing::ElementsAre(std::addressof(plus_op_instance), plus_op_module,
                           plus_op_module, std::addressof(plus_op_instance)));
}

}  // namespace
}  // namespace raksha::ir
