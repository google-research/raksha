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

#include <limits>

#include "src/backends/policy_engine/dp_parameter_policy.h"
#include "src/backends/policy_engine/sql_policy_rule_policy.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/ops/literal_op.h"
#include "src/frontends/sql/ops/sql_output_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::backends::policy_engine {
namespace {

using ir::NamedAttributeMap;
using ir::Operation;
using ir::Operator;
using ir::ValueList;

ir::Value MakeOperationResultValue(const Operation &op) {
  return ir::Value(ir::value::OperationResult(op, "out"));
}

TEST(SoufflePolicyCheckerTest, SqlPolicyRuleReturnsTrue) {
  SoufflePolicyChecker checker;
  ir::Module module;
  EXPECT_TRUE(checker.IsModulePolicyCompliant(module, SqlPolicyRulePolicy("")));
}

TEST(SoufflePolicyCheckerTest, SqlPolicyRuleReturnsFalse) {
  ir::IRContext ir_context;
  ir_context.RegisterOperator(std::make_unique<Operator>(
      frontends::sql::OpTraits<frontends::sql::LiteralOp>::kName));
  ir_context.RegisterOperator(std::make_unique<Operator>(
      frontends::sql::OpTraits<frontends::sql::SqlOutputOp>::kName));
  ir_context.RegisterOperator(std::make_unique<Operator>(
      frontends::sql::OpTraits<frontends::sql::TagTransformOp>::kName));
  SoufflePolicyChecker checker;
  ir::BlockBuilder block_builder;
  const Operation &literal =
      block_builder.AddOperation<frontends::sql::LiteralOp>(ir_context,
                                                            "some_literal");
  const Operation &tag_transform =
      block_builder.AddOperation<frontends::sql::TagTransformOp>(
          ir_context, "taint_rule", MakeOperationResultValue(literal),
          std::vector<std::pair<std::string, ir::Value>>());

  block_builder.AddOperation<frontends::sql::SqlOutputOp>(
      ir_context, MakeOperationResultValue(tag_transform));

  ir::Module module;
  module.AddBlock(block_builder.build());

  // Expect that the module is not policy compliant because it puts taint
  // directly into a sql output op.
  EXPECT_FALSE(checker.IsModulePolicyCompliant(
      module, SqlPolicyRulePolicy(
                  R"(["taint_rule", $AddConfidentialityTag("taint"), nil])")));
  // But expect that without any policy rules, the tag transform will not fire
  // and it will be policy compliant.
  EXPECT_TRUE(checker.IsModulePolicyCompliant(module, SqlPolicyRulePolicy("")));
}

TEST(SoufflePolicyCheckerTest, DpPolicyRuleReturnsTrue) {
  SoufflePolicyChecker checker;
  ir::Module module;
  EXPECT_TRUE(
      checker.IsModulePolicyCompliant(module, DpParameterPolicy(10, 10)));
}

// The graph we construct here connects an input table to a SQL output, with
// the results being grouped, going through a privacy mechanism, and being
// averaged before being output. The privacy mechanism has an epsilon of
// 5. We show that this result passes with a global epsilon limit of 10 and
// fails with a global epsilon limit of 9 (as group by doubles the
// sensitivity of the epsilon-5 privacy mechanism).
TEST(SoufflePolicyCheckerTest, DpPolicyRuleReturnsFalse) {
  SoufflePolicyChecker checker;
  ir::IrProgramParser ir_parser;
  ir::IrProgramParser::Result parse_result = ir_parser.ParseProgram(R"(
module m0 {
block b0 {
%0 = core.input[name: "MyTable"]()
%1 = sql.group_by[](%0)
%2 = privacy_mechanism[epsilon: 5](%1)
%3 = sql.average[](%2)
%4 = sql.sql_output[](%3)
} })");

  const ir::Module &module = *parse_result.module;
  EXPECT_FALSE(
      checker.IsModulePolicyCompliant(module, DpParameterPolicy(9, 0)));
  EXPECT_TRUE(
      checker.IsModulePolicyCompliant(module, DpParameterPolicy(10, 1000)));
}

}  // anonymous namespace
}  // namespace raksha::backends::policy_engine
