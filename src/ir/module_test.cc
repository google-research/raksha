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
#include "src/ir/module.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {
TEST(ModuleTest, DefaultConstructorCreatesEmptyBlocks) {
  Module module;
  EXPECT_EQ(module.blocks().size(), 0);
}

TEST(ModuleTest, AddBlockReturnsPassedInBlock) {
  Module module;
  auto block = std::make_unique<Block>();
  const Block* passed_in_block = block.get();
  const Block& returned_block = module.AddBlock(std::move(block));
  EXPECT_EQ(std::addressof(returned_block), passed_in_block);
  EXPECT_EQ(returned_block.module(), &module);
}

TEST(ModuleTest, AddBlockUpdatesBlockList) {
  Module module;
  const Block* block1 =
      std::addressof(module.AddBlock(std::make_unique<Block>()));
  const Block* block2 =
      std::addressof(module.AddBlock(std::make_unique<Block>()));
  EXPECT_NE(block1, block2);
  EXPECT_THAT(module.blocks(), testing::UnorderedElementsAre(
                                   testing::Pointer(testing::Eq(block1)),
                                   testing::Pointer(testing::Eq(block2))));
}

}  // namespace
}  // namespace raksha::ir
