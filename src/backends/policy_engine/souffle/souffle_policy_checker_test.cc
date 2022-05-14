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
#include "src/backends/policy_engine/souffle/souffle_policy_checker.h"

#include "src/backends/policy_engine/souffle/datalog_fact_policy.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/ops/literal_op.h"
#include "src/frontends/sql/ops/sql_output_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"

namespace raksha::backends::policy_engine {
namespace {

TEST(SoufflePolicyCheckerTest, ReturnsTrue) {
  SoufflePolicyChecker checker;
  ir::Module module;
  souffle::SqlPolicyRuleFactPolicy policy("");
  EXPECT_TRUE(checker.IsModulePolicyCompliant(module, policy));
}

TEST(SoufflePolicyCheckerTest, ReturnsFalse) {
  ir::IRContext ir_context;
  ir_context.RegisterOperator(std::make_unique<ir::Operator>(
      frontends::sql::OpTraits<frontends::sql::LiteralOp>::kName));
  ir_context.RegisterOperator(std::make_unique<ir::Operator>(
      frontends::sql::OpTraits<frontends::sql::SqlOutputOp>::kName));
  ir_context.RegisterOperator(std::make_unique<ir::Operator>(
      frontends::sql::OpTraits<frontends::sql::TagTransformOp>::kName));
  SoufflePolicyChecker checker;
  ir::BlockBuilder block_builder;
  const ir::Operation &literal =
      block_builder.AddOperation<frontends::sql::LiteralOp>(ir_context,
                                                            "some_literal");
  const ir::Operation &tag_transform =
      block_builder.AddOperation<frontends::sql::TagTransformOp>(
          ir_context, "taint_rule",
          ir::Value(ir::value::OperationResult(literal, "out")),
          std::vector<std::pair<std::string, ir::Value>>());

  block_builder.AddOperation<frontends::sql::SqlOutputOp>(
      ir_context, ir::Value(ir::value::OperationResult(tag_transform, "out")));

  ir::Module module;
  module.AddBlock(block_builder.build());

  souffle::SqlPolicyRuleFactPolicy policy(
      R"(["taint_rule", $AddConfidentialityTag("taint"), nil])");
  // Expect that the module is not policy compliant because it puts taint
  // directly into a sql output op.
  EXPECT_FALSE(checker.IsModulePolicyCompliant(module, policy));
  // But expect that without any policy rules, the tag transform will not fire
  // and it will be policy compliant.
  EXPECT_TRUE(checker.IsModulePolicyCompliant(
      module, souffle::SqlPolicyRuleFactPolicy("")));
}

}  // anonymous namespace
}  // namespace raksha::backends::policy_engine
