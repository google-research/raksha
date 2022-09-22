/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "src/ir/auth_logic/souffle_emitter.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/lowering_ast_datalog.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

Program BuildSingleAssertionProgram(SaysAssertion assertion) {
  std::vector<SaysAssertion> assertion_list = {};
  assertion_list.push_back(std::move(assertion));
  return Program({}, std::move(assertion_list), {});
}

SaysAssertion BuildSingleSaysAssertion(Principal speaker, Assertion assertion) {
  std::vector<Assertion> assertion_list = {};
  assertion_list.push_back(std::move(assertion));
  return SaysAssertion(std::move(speaker), std::move(assertion_list));
}

Program BuildPredicateTestProgram() {
  return BuildSingleAssertionProgram(BuildSingleSaysAssertion(
      Principal("TestPrincipal"),
      Assertion(Fact({}, BaseFact(datalog::Predicate("foo", {"bar", "baz"},
                                                     datalog::kPositive))))));
}

Program BuildRelationDeclarationProgram(SaysAssertion assertion) {
  std::vector<SaysAssertion> assertion_list = {};
  assertion_list.push_back(std::move(assertion));
  std::vector<datalog::RelationDeclaration> relation_declaration = {
      datalog::RelationDeclaration(
          "grantAccess", false,
          {datalog::Argument(
               "x0", datalog::ArgumentType::MakePrincipalType()),
           datalog::Argument(
               "x1", datalog::ArgumentType::MakeCustomType("FileName"))})};
  return Program(std::move(relation_declaration), std::move(assertion_list),
                 {});
}

TEST(EmitterTestSuite, EmptyAuthLogicTest) {
  std::string expected =
      R"(.type DummyType <: symbol
.type Number <: symbol
.decl grounded_dummy(dummy_param : DummyType)
.decl says_canActAs(speaker : Principal, p1 : Principal, p2 : Principal)
.decl says_isNumber(speaker : Principal, x : Number)
grounded_dummy("dummy_var").
)";
  std::string actual = SouffleEmitter::EmitProgram(
      LoweringToDatalogPass::Lower(Program({}, {}, {})));
  EXPECT_EQ(actual, expected);
}

TEST(EmitterTestSuite, SimpleTest) {
  std::string expected =
      R"(.type DummyType <: symbol
.type Number <: symbol
.decl grounded_dummy(dummy_param : DummyType)
.decl says_canActAs(speaker : Principal, p1 : Principal, p2 : Principal)
.decl says_isNumber(speaker : Principal, x : Number)
says_foo(TestPrincipal, bar, baz).
grounded_dummy("dummy_var").
)";

  std::string actual = SouffleEmitter::EmitProgram(
      LoweringToDatalogPass::Lower(BuildPredicateTestProgram()));

  EXPECT_EQ(actual, expected);
}

Program BuildCanSayProgram() {
  return BuildRelationDeclarationProgram(BuildSingleSaysAssertion(
      Principal("TestSpeaker"),
      Assertion(Fact({Principal("PrincipalA")},
                     BaseFact(datalog::Predicate("grantAccess", {"secretFile"},
                                                 datalog::kPositive))))));
}

TEST(EmitterTestSuite, CanSayTest) {
  std::string expected =
      R"(.type DummyType <: symbol
.type FileName <: symbol
.type Number <: symbol
.decl grounded_dummy(dummy_param : DummyType)
.decl says_canActAs(speaker : Principal, p1 : Principal, p2 : Principal)
.decl says_canSay_grantAccess(speaker : Principal, delegatee1 : Principal, x0 : Principal, x1 : FileName)
.decl says_grantAccess(speaker : Principal, x0 : Principal, x1 : FileName)
.decl says_isNumber(speaker : Principal, x : Number)
says_grantAccess(TestSpeaker, secretFile) :- says_grantAccess(x__1, secretFile), says_canSay_grantAccess(TestSpeaker, x__1, secretFile).
says_canSay_grantAccess(TestSpeaker, PrincipalA, secretFile).
grounded_dummy("dummy_var").
)";

  std::string actual = SouffleEmitter::EmitProgram(
      LoweringToDatalogPass::Lower(BuildCanSayProgram()));

  EXPECT_EQ(actual, expected);
}

Program BuildFloatCanSayProgram() {
  BaseFact inner_base_fact(
      datalog::Predicate("grantAccess", {"secretFile"}, datalog::kPositive));
  Fact cansay_fact({Principal("PrincipalA"), Principal("PrincipalB")},
                   inner_base_fact);
  return BuildRelationDeclarationProgram(BuildSingleSaysAssertion(
      Principal("TestSpeaker"), Assertion(Fact(std::move(cansay_fact)))));
}

TEST(EmitterTestSuite, FloatCanSayTest) {
  std::string expected =
      R"(.type DummyType <: symbol
.type FileName <: symbol
.type Number <: symbol
.decl grounded_dummy(dummy_param : DummyType)
.decl says_canActAs(speaker : Principal, p1 : Principal, p2 : Principal)
.decl says_canSay_canSay_grantAccess(speaker : Principal, delegatee2 : Principal, delegatee1 : Principal, x0 : Principal, x1 : FileName)
.decl says_canSay_grantAccess(speaker : Principal, delegatee1 : Principal, x0 : Principal, x1 : FileName)
.decl says_grantAccess(speaker : Principal, x0 : Principal, x1 : FileName)
.decl says_isNumber(speaker : Principal, x : Number)
says_grantAccess(TestSpeaker, secretFile) :- says_grantAccess(x__1, secretFile), says_canSay_grantAccess(TestSpeaker, x__1, secretFile).
says_canSay_grantAccess(TestSpeaker, PrincipalA, secretFile) :- says_canSay_grantAccess(x__2, PrincipalA, secretFile), says_canSay_canSay_grantAccess(TestSpeaker, x__2, PrincipalA, secretFile).
says_canSay_canSay_grantAccess(TestSpeaker, PrincipalB, PrincipalA, secretFile).
grounded_dummy("dummy_var").
)";

  std::string actual = SouffleEmitter::EmitProgram(
      LoweringToDatalogPass::Lower(BuildFloatCanSayProgram()));

  EXPECT_EQ(actual, expected);
}

}  // namespace raksha::ir::auth_logic
