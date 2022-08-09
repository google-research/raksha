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

#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"

#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {
namespace {

// A visitor that makes a set of all the names of principals in the program
class PrincipalNameCollectorVisitor
    : public AuthLogicAstTraversingVisitor<PrincipalNameCollectorVisitor,
                                           absl::flat_hash_set<std::string>> {
 public:
  PrincipalNameCollectorVisitor() {}

  absl::flat_hash_set<std::string> GetDefaultValue() override { return {}; }

  absl::flat_hash_set<std::string> FoldResult(
      absl::flat_hash_set<std::string> acc,
      absl::flat_hash_set<std::string> child_result) override {
    acc.merge(std::move(child_result));
    return std::move(acc);
  }

  absl::flat_hash_set<std::string> PreVisit(
      const Principal& principal) override {
    return {principal.name()};
  }
};

Program BuildTestProgram1() {
  SaysAssertion assertion1 = SaysAssertion(
      Principal("PrincipalA"),
      {Assertion(Fact({}, BaseFact(datalog::Predicate("foo", {"bar", "baz"},
                                                      datalog::kPositive))))});
  SaysAssertion assertion2 = SaysAssertion(
      Principal("PrincipalA"),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("foo", {"barbar", "bazbaz"},
                                               datalog::kPositive))))});
  SaysAssertion assertion3 = SaysAssertion(
      Principal("PrincipalB"),
      {Assertion(Fact({}, BaseFact(CanActAs(Principal("PrincipalA"),
                                            Principal("PrincipalC")))))});
  std::vector<SaysAssertion> assertion_list = {
      std::move(assertion1), std::move(assertion2), std::move(assertion3)};
  return Program({}, std::move(assertion_list), {});
}

TEST(AuthLogicAstTraversingVisitorTest, PrincipalNameCollectorTest) {
  Program test_prog = BuildTestProgram1();
  PrincipalNameCollectorVisitor collector_visitor;
  const absl::flat_hash_set<std::string> result =
      test_prog.Accept(collector_visitor);
  const absl::flat_hash_set<std::string> expected = {"PrincipalA", "PrincipalB",
                                                     "PrincipalC"};
  EXPECT_EQ(result, expected);
}

}  // namespace
}  // namespace raksha::ir::auth_logic
