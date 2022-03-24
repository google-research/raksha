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

#include "src/ir/auth_logic/ast_construction.h"

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

struct QueryTestData {
  std::string input_text;
  std::string query_name;
  std::string principal_name;
  std::string fact_name;
  datalog::Predicate predicate;
};

class AstConstructionTest : public testing::TestWithParam<QueryTestData> {};

TEST_P(AstConstructionTest, SimpleTestWithQueryProgram) {
  const auto& [input_text, query_name, principal_name, fact_name, predicate] =
      GetParam();
  Program prog_out = ParseProgram(input_text);
  ASSERT_EQ(prog_out.queries().size(), 1);
  const Query& query = prog_out.queries().front();
  EXPECT_EQ(query.name(), query_name);
  EXPECT_EQ(query.principal().name(), principal_name);
  EXPECT_EQ(std::get<datalog::Predicate>(
                std::get<BaseFact>(query.fact().GetValue()).GetValue())
                .name(),
            fact_name);
  EXPECT_EQ(std::get<datalog::Predicate>(
                std::get<BaseFact>(query.fact().GetValue()).GetValue()),
            predicate);
}

INSTANTIATE_TEST_SUITE_P(
    AstConstructionTest, AstConstructionTest,
    testing::Values(
        QueryTestData(
            {R"(may_notifierA = query "UserA" says mayA("NotifierA"))",
             "may_notifierA", R"("UserA")", "mayA",
             datalog::Predicate("mayA", {R"("NotifierA")"},
                                datalog::Sign::kPositive)}),
        QueryTestData(
            {R"(may_notifierB = query "UserB" says !mayB("NotifierB"))",
             "may_notifierB", R"("UserB")", "mayB",
             datalog::Predicate("mayB", {R"("NotifierB")"},
                                datalog::Sign::kNegated)})));

}  // namespace raksha::ir::auth_logic
