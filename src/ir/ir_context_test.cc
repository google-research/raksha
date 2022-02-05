//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
#include "src/ir/ir_context.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

TEST(IRContextTest, RegisterOperatorReturnsRegisteredOp) {
  IRContext context;
  const Operator& op =
      context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  EXPECT_EQ(op.name(), "core.choose");
}

TEST(IRContextTest, GetOperatorReturnsRegisteredOperator) {
  IRContext context;
  const Operator& registered_op =
      context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  const Operator* op = context.GetOperator("core.choose");
  EXPECT_EQ(op->name(), "core.choose");
  EXPECT_EQ(op, &registered_op);
}

TEST(IRContextTest, GetOperatorReturnsNullptrForUnregisteredOperator) {
  IRContext context;
  const Operator* op = context.GetOperator("core.choose");
  EXPECT_EQ(op, nullptr);
}

TEST(IRContextDeathTest, DuplicateRegistrationCausesFailure) {
  IRContext context;
  context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  EXPECT_DEATH(
      { context.RegisterOperator(std::make_unique<Operator>("core.choose")); },
      "Cannot register duplicate operator with name 'core.choose'.");
}

}  // namespace
}  // namespace raksha::ir
