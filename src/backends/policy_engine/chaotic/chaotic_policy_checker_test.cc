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
#include "src/backends/policy_engine/chaotic/chaotic_policy_checker.h"

#include "src/common/testing/gtest.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::backends::policy_engine {
namespace {

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

// Field 0 is the rule name, field 1 is the result of the rule executing, and
// field 2 is the preconditions
//
// ["<no schema>/People/SSN/integrity/SocialSecurityNumber",
// $AddIntegrityTag("SocialSecurityNumber"), nil]
// ["<no schema>/People/SSN/confidentiality/anon_tag_confidentiality_0",
// $AddConfidentialityTag("anon_tag_confidentiality_0"), nil]
// ["<no schema>/BirthRecords/SSN/integrity/SocialSecurityNumber",
// $AddIntegrityTag("SocialSecurityNumber"), nil]
// ["<no schema>/BirthRecords/SSN/confidentiality/anon_tag_confidentiality_0",
// $AddConfidentialityTag("anon_tag_confidentiality_0"), nil]
// ["<no schema>/BirthRecords/birthTimestamp/integrity/TimestampMillis",
// $AddIntegrityTag("TimestampMillis"), nil]
// ["<no
// schema>/BirthRecords/birthTimestamp/confidentiality/anon_tag_confidentiality_0",
// $AddConfidentialityTag("anon_tag_confidentiality_0"), nil]
// ["Rule_0", $RemoveConfidentialityTag("anon_tag_confidentiality_0"), [["arg_",
// "TimestampMillis"], nil]]
// ["Rule_1", $RemoveConfidentialityTag("anon_tag_confidentiality_0"),
// [["arg_a", "SocialSecurityNumber"], [["arg_b", "SocialSecurityNumber"],
// nil]]]

}  // anonymous namespace
}  // namespace raksha::backends::policy_engine
