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
#include "src/frontends/sql/ops/literal_op.h"

#include <algorithm>

#include "src/common/testing/gtest.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/string_attribute.h"

namespace raksha::frontends::sql {
namespace {

TEST(LiteralOpTest, FactoryMethodCreatesCorrectOperation) {
  ir::IRContext context;
  SqlOp::RegisterOperator<LiteralOp>(context);

  std::unique_ptr<ir::Operation> op =
      LiteralOp::Create(nullptr, context, "hello");
  EXPECT_EQ(op->op().name(), OpTraits<LiteralOp>::kName);
  EXPECT_EQ(op->parent(), nullptr);
  EXPECT_TRUE(op->inputs().empty());
  EXPECT_THAT(op->attributes(),
              testing::UnorderedElementsAre(testing::Pair(
                  std::string(LiteralOp::kLiteralStringAttrName),
                  ir::Attribute::Create<ir::StringAttribute>("hello"))));
}

static const ir::IRContext kContext({ir::Operator(OpTraits<LiteralOp>::kName)},
                                    {});
static LiteralOp kGlobalLiteralOp(nullptr, kContext, "hello");

TEST(LiteralOpTest, CtorCreatesCorrectOperation) {
  EXPECT_EQ(kGlobalLiteralOp.op().name(), OpTraits<LiteralOp>::kName);
  EXPECT_EQ(kGlobalLiteralOp.parent(), nullptr);
  EXPECT_TRUE(kGlobalLiteralOp.inputs().empty());
  EXPECT_THAT(kGlobalLiteralOp.attributes(),
              testing::UnorderedElementsAre(testing::Pair(
                  std::string(LiteralOp::kLiteralStringAttrName),
                  ir::Attribute::Create<ir::StringAttribute>("hello"))));
}

}  // namespace
}  // namespace raksha::frontends::sql
