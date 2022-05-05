//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/ast_construction.h"
#include "src/ir/auth_logic/ast_string.h"

namespace raksha::ir::auth_logic {

using ::testing::UnorderedElementsAre;

struct QueryTestData {
  std::string input_text;
  std::string query_name;
  std::string principal;
  std::string fact;
};

std::string ToString(datalog::Predicate predicate) {
  return absl::StrCat(predicate.sign() == datalog::Sign::kNegated ? "!" : "",
                      predicate.name(), "(",
                      absl::StrJoin(predicate.args(), ", "), ")");
}
std::string ToString(BaseFact basefact) {
  return std::visit(
      raksha::utils::overloaded{
          [](datalog::Predicate predicate) { return ToString(predicate); },
          [](Attribute attribute) {
            return absl::StrJoin(
                {attribute.principal().name(), ToString(attribute.predicate())},
                " ");
          },
          [](CanActAs can_act_as) {
            return absl::StrJoin({can_act_as.left_principal().name(),
                                  can_act_as.right_principal().name()},
                                 " canActAs ");
          }},
      basefact.GetValue());
}

std::string ToString(Fact fact) {
  if (fact.delegation_chain().empty()) {
    return ToString(fact.base_fact());
  }
  std::string cansay_string = "";
  for (const Principal& delegatees : fact.delegation_chain()) {
    cansay_string =
        absl::StrJoin({delegatees.name(), cansay_string}, " canSay ");
  }
  return absl::StrJoin({cansay_string, ToString(fact.base_fact())}, " ");
}

class QueryAstConstructionTest : public testing::TestWithParam<QueryTestData> {
};

TEST_P(QueryAstConstructionTest, SimpleTestWithQueryProgram) {
  const auto& [input_text, query_name, principal, fact] = GetParam();
  Program prog_out = ParseProgram(input_text);
  ASSERT_EQ(prog_out.queries().size(), 1);
  const Query& query = prog_out.queries().front();
  EXPECT_EQ(query.name(), query_name);
  EXPECT_EQ(query.principal().name(), principal);
  EXPECT_EQ(ToString(query.fact()), fact);
}

INSTANTIATE_TEST_SUITE_P(
    QueryAstConstructionTest, QueryAstConstructionTest,
    testing::Values(
        QueryTestData({R"(may_notifierA = query "UserA" says mayA(NotifierA))",
                       "may_notifierA", R"("UserA")", "mayA(NotifierA)"}),
        QueryTestData({R"(may_notifierB = query "UserB" says !mayB(NotifierB))",
                       "may_notifierB", R"("UserB")", "!mayB(NotifierB)"})));

}  // namespace
   // raksha::ir::auth_logic
