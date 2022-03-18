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
#include "src/frontends/sql/ops/sql_op.h"

#include <memory>

#include "src/common/testing/gtest.h"
#include "src/frontends/sql/ops/op_traits.h"
#include "src/ir/ir_context.h"

namespace raksha::frontends::sql {
namespace {

TEST(SqlOpTest, RegisterOperatorRegistersInContext) {
  ir::IRContext context;
  absl::string_view literal_op_name = OpTraits<LiteralOp>::kName;
  EXPECT_FALSE(context.IsRegisteredOperator(literal_op_name));

  SqlOp::RegisterOperator<LiteralOp>(context);
  EXPECT_TRUE(context.IsRegisteredOperator(literal_op_name));

  const ir::Operator& op = context.GetOperator(literal_op_name);
  EXPECT_EQ(op.name(), literal_op_name);
}

TEST(SqlOpTest, GetOperatorReturnsRegisteredOperatorInContext) {
  ir::IRContext context;
  absl::string_view literal_op_name = OpTraits<LiteralOp>::kName;

  EXPECT_FALSE(context.IsRegisteredOperator(literal_op_name));
  context.RegisterOperator(std::make_unique<ir::Operator>(literal_op_name));
  EXPECT_TRUE(context.IsRegisteredOperator(literal_op_name));

  const ir::Operator& op = SqlOp::GetOperator<LiteralOp>(context);
  EXPECT_EQ(op.name(), literal_op_name);
}

TEST(SqlOpDeathTest, GettingUnregisteredOperatorFails) {
  EXPECT_DEATH(
      {
        ir::IRContext context;
        SqlOp::GetOperator<LiteralOp>(context);
      },
      "SqlOp 'sql.literal' is not registered.");
}

}  // namespace
}  // namespace raksha::frontends::sql
