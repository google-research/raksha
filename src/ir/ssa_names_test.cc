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
#include "src/ir/ssa_names.h"

#include "src/common/testing/gtest.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::ir {
namespace {

class SsaNamesTest : public ::testing::Test {
 public:
  SsaNamesTest() : test_op_("test") {}

 protected:
  Operator test_op_;
};

TEST_F(SsaNamesTest, GetOrCreateIDReturnsUniqueIDsForOperations) {
  SsaNames names;
  Operation first_operation(nullptr, test_op_, {});
  Operation second_operation(nullptr, test_op_, {});
  SsaNames::ID first_operation_id = names.GetOrCreateID(first_operation);
  SsaNames::ID second_operation_id = names.GetOrCreateID(second_operation);

  EXPECT_NE(first_operation_id, second_operation_id);
  EXPECT_EQ(names.GetOrCreateID(first_operation), first_operation_id);
}

TEST_F(SsaNamesTest, GetOrCreateIDReturnsUniqueIDsForBlocks) {
  SsaNames names;
  Block first_block;
  Block second_block;
  SsaNames::ID first_block_id = names.GetOrCreateID(first_block);
  SsaNames::ID second_block_id = names.GetOrCreateID(second_block);

  EXPECT_NE(first_block_id, second_block_id);
  EXPECT_EQ(names.GetOrCreateID(first_block), first_block_id);
}

}  // namespace
}  // namespace raksha::ir
