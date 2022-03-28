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
#include "src/ir/attributes/attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

namespace {

TEST(SqlOpTest, RegisterOperatorRegistersInContext) {
  ir::IRContext context;
  absl::string_view literal_op_name = OpTraits<LiteralOp>::kName;
  EXPECT_FALSE(context.IsRegisteredOperator(literal_op_name));

  SqlOp::RegisterOperator<LiteralOp>(context);
  EXPECT_TRUE(context.IsRegisteredOperator(literal_op_name));

  const ir::Operator* op = context.GetOperator(literal_op_name);
  ASSERT_NE(op, nullptr);
  EXPECT_EQ(op->name(), literal_op_name);
}

TEST(SqlOpTest, GetOperatorReturnsRegisteredOperatorInContext) {
  ir::IRContext context;
  absl::string_view literal_op_name = OpTraits<LiteralOp>::kName;

  EXPECT_FALSE(context.IsRegisteredOperator(literal_op_name));
  context.RegisterOperator(std::make_unique<ir::Operator>(literal_op_name));
  EXPECT_TRUE(context.IsRegisteredOperator(literal_op_name));

  const ir::Operator* op = SqlOp::GetOperator<LiteralOp>(context);
  ASSERT_NE(op, nullptr);
  EXPECT_EQ(op->name(), literal_op_name);
}

TEST(SqlOpTest, GettingUnregisteredReturnsNullptr) {
  ir::IRContext context;
  EXPECT_EQ(SqlOp::GetOperator<LiteralOp>(context), nullptr);
}

}  // namespace

// Define some ops for testing.

class TestOp : public SqlOp {
 public:
  absl::string_view value() const { return "TestOp"; }
};
template <>
struct OpTraits<TestOp> {
  static constexpr absl::string_view kName = "sql.this_is_a_test_op";
};

class AnotherTestOp : public SqlOp {
 public:
  absl::string_view value() const { return "AnotherTestOp"; }
};
template <>
struct OpTraits<AnotherTestOp> {
  static constexpr absl::string_view kName = "sql.this_is_another_test_op";
};

namespace {

class SqlOpGetIfTest : public testing::Test {
 public:
  SqlOpGetIfTest() {
    SqlOp::RegisterOperator<TestOp>(context_);
    SqlOp::RegisterOperator<AnotherTestOp>(context_);
    test_operation_ = std::make_unique<ir::Operation>(
        nullptr, *CHECK_NOTNULL(SqlOp::GetOperator<TestOp>(context_)),
        ir::NamedAttributeMap({}), ir::ValueList({}));
    another_test_operation_ = std::make_unique<ir::Operation>(
        nullptr, *CHECK_NOTNULL(SqlOp::GetOperator<AnotherTestOp>(context_)),
        ir::NamedAttributeMap({}), ir::ValueList({}));
  }

 protected:
  ir::IRContext context_;
  std::unique_ptr<ir::Operation> test_operation_;
  std::unique_ptr<ir::Operation> another_test_operation_;
};

TEST_F(SqlOpGetIfTest, GetIfReturnsNonNullIfCorrectOpType) {
  const TestOp* test_operation = SqlOp::GetIf<TestOp>(*test_operation_);
  EXPECT_NE(test_operation, nullptr);
  EXPECT_EQ(test_operation->value(), "TestOp");

  const AnotherTestOp* another_test_operation =
      SqlOp::GetIf<AnotherTestOp>(*another_test_operation_);
  EXPECT_NE(another_test_operation, nullptr);
  EXPECT_EQ(another_test_operation->value(), "AnotherTestOp");
}

TEST_F(SqlOpGetIfTest, GetIfReturnsNullIfIncorrectOpType) {
  const AnotherTestOp* test_op = SqlOp::GetIf<AnotherTestOp>(*test_operation_);
  EXPECT_EQ(test_op, nullptr);

  const TestOp* another_test_op =
      SqlOp::GetIf<TestOp>(*another_test_operation_);
  EXPECT_EQ(another_test_op, nullptr);
}

}  // namespace

}  // namespace raksha::frontends::sql
