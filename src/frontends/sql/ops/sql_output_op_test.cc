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
#include "src/frontends/sql/ops/sql_output_op.h"

#include "src/common/testing/gtest.h"
#include "src/common/utils/ranges.h"
#include "src/frontends/sql/ops/example_value_test_helper.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {
namespace {

using ::testing::Values;

class SqlOutputOpTest : public ::testing::TestWithParam<ir::Value> {
 public:
  SqlOutputOpTest() { SqlOp::RegisterOperator<SqlOutputOp>(context_); }

 protected:
  ir::IRContext context_;
};

TEST_P(SqlOutputOpTest, FactoryMethodCreatesCorrectOperation) {
  const ir::Value wrapped_value = GetParam();

  std::unique_ptr<ir::Operation> op =
      SqlOutputOp::Create(nullptr, context_, wrapped_value);
  EXPECT_EQ(op->op().name(), OpTraits<SqlOutputOp>::kName);
  EXPECT_EQ(op->parent(), nullptr);
  EXPECT_EQ(op->inputs().size(), 1);
  EXPECT_EQ(op->inputs().front(), wrapped_value);
  EXPECT_TRUE(op->attributes().empty());
}


TEST_P(SqlOutputOpTest, GetWrappedValueTest) {
   const ir::Value wrapped_value = GetParam();

  std::unique_ptr<SqlOutputOp> op =
      SqlOutputOp::Create(nullptr, context_, wrapped_value);
  EXPECT_EQ(op->GetValueMarkedAsOutput(), wrapped_value);
}

const ir::Operator kSomeOp("some_op");
const ir::Operation kExampleOperation = ir::Operation(nullptr, kSomeOp, {}, {});

INSTANTIATE_TEST_SUITE_P(
    SqlOutputOpTest, SqlOutputOpTest,
    Values(ir::Value(ir::value::Any()),
           ir::Value(ir::value::OperationResult(kExampleOperation, 0))));
}
}  // namespace raksha::frontends::sql::ops
