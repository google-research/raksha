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

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/datalog_ir.h"
#include "src/ir/auth_logic/lowering_ast_datalog.h"
#include "src/ir/auth_logic/souffle_emitter.h"
#include "src/ir/auth_logic/move_append.h"

namespace raksha::ir::auth_logic {

    Program BuildSingleAssertionProgram(SaysAssertion assertion) {
      std::vector<SaysAssertion> assertion_list = {};
      assertion_list.push_back(std::move(assertion));
      return Program(std::move(assertion_list), {});
    }

    SaysAssertion BuildSingleSaysAssertion(Principal speaker, Assertion assertion) {
      std::vector<Assertion> assertion_list = {};
      assertion_list.push_back(std::move(assertion));
      return SaysAssertion(std::move(speaker), std::move(assertion_list));
    }
    
    Program BuildPredicateTestProgram() {
      return BuildSingleAssertionProgram(BuildSingleSaysAssertion(
            Principal("TestPrincipal"), Assertion(Fact(BaseFact(
              Predicate("foo", {"bar", "baz"}, kPositive))))));
    }

    TEST(EmitterTestSuite, SimpleTest) {
      std::string expected =
R"(says_foo(TestPrincipal, bar, baz).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_foo(x0: symbol, x1: symbol, x2: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildPredicateTestProgram()));

      EXPECT_EQ(actual, expected);
    }

    Program BuildAttributeTestProgram() {
      return BuildSingleAssertionProgram(
          BuildSingleSaysAssertion(Principal("TestSpeaker"),
              Assertion(Fact(BaseFact(
                    Attribute(Principal("OtherTestPrincipal"),
                      Predicate("hasProperty", {"wellTested"}, kPositive)))))
              ));
    }

    TEST(EmitterTestSuite, AttributeTest) {
      std::string expected =
R"(says_hasProperty(TestSpeaker, x__1, wellTested) :- says_canActAs(TestSpeaker, x__1, OtherTestPrincipal), says_hasProperty(TestSpeaker, OtherTestPrincipal, wellTested).
says_hasProperty(TestSpeaker, OtherTestPrincipal, wellTested).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canActAs(x0: symbol, x1: symbol, x2: symbol)
.decl says_hasProperty(x0: symbol, x1: symbol, x2: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildAttributeTestProgram()));

      EXPECT_EQ(actual, expected);
    }

    Program BuildCanActAsProgram() {
      return BuildSingleAssertionProgram(
          BuildSingleSaysAssertion(Principal("TestSpeaker"),
            Assertion(Fact(BaseFact(
                  CanActAs(Principal("PrincipalA"), Principal("PrincipalB")))))
            ));
    }

    TEST(EmitterTestSuite, CanActAsTest) {
      std::string expected =
R"(says_canActAs(TestSpeaker, x__1, PrincipalB) :- says_canActAs(TestSpeaker, x__1, PrincipalA), says_canActAs(TestSpeaker, PrincipalA, PrincipalB).
says_canActAs(TestSpeaker, PrincipalA, PrincipalB).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canActAs(x0: symbol, x1: symbol, x2: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildCanActAsProgram()));

      EXPECT_EQ(actual, expected);
    }

    Program BuildCanSayProgram() {
      std::unique_ptr<Fact> inner_fact = std::unique_ptr<Fact>(
          new Fact(BaseFact(Predicate("grantAccess",
              {"secretFile"}, kPositive))));
      Fact::FactVariantType cansay_fact = std::unique_ptr<CanSay>(
          new CanSay(Principal("PrincipalA"), inner_fact));
      return BuildSingleAssertionProgram(
          BuildSingleSaysAssertion(Principal("TestSpeaker"),
            Assertion(Fact(std::move(cansay_fact)))));
    }

    TEST(EmitterTestSuite, CanSayTest) {
      std::string expected =
R"(says_grantAccess(TestSpeaker, secretFile) :- says_grantAccess(x__1, secretFile), says_canSay_grantAccess(TestSpeaker, x__1, secretFile).
says_canSay_grantAccess(TestSpeaker, PrincipalA, secretFile).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canSay_grantAccess(x0: symbol, x1: symbol, x2: symbol)
.decl says_grantAccess(x0: symbol, x1: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildCanSayProgram()));

      EXPECT_EQ(actual, expected);
    }

    Program BuildDoubleCanSayProgram() {
      // So much fun with unique pointers!
      std::unique_ptr<Fact> inner_fact = std::unique_ptr<Fact>(
          new Fact(BaseFact(Predicate("grantAccess",
              {"secretFile"}, kPositive))));
      Fact::FactVariantType inner_cansay = std::unique_ptr<CanSay>(
          new CanSay(Principal("PrincipalA"), inner_fact));
      std::unique_ptr<Fact> inner_cansay_fact = std::unique_ptr<Fact>(
          new Fact(std::move(inner_cansay)));
      Fact::FactVariantType cansay_fact = std::unique_ptr<CanSay>(
          new CanSay(Principal("PrincipalB"), inner_cansay_fact));
      return BuildSingleAssertionProgram(
          BuildSingleSaysAssertion(Principal("TestSpeaker"),
            Assertion(Fact(std::move(cansay_fact)))));
    }

    TEST(EmitterTestSuite, DoubleCanSayTest) {
      std::string expected = 
R"(says_grantAccess(TestSpeaker, secretFile) :- says_grantAccess(x__1, secretFile), says_canSay_grantAccess(TestSpeaker, x__1, secretFile).
says_canSay_grantAccess(TestSpeaker, PrincipalA, secretFile) :- says_canSay_grantAccess(x__2, PrincipalA, secretFile), says_canSay_canSay_grantAccess(TestSpeaker, x__2, PrincipalA, secretFile).
says_canSay_canSay_grantAccess(TestSpeaker, PrincipalB, PrincipalA, secretFile).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canSay_canSay_grantAccess(x0: symbol, x1: symbol, x2: symbol, x3: symbol)
.decl says_canSay_grantAccess(x0: symbol, x1: symbol, x2: symbol)
.decl says_grantAccess(x0: symbol, x1: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildDoubleCanSayProgram()));

      EXPECT_EQ(actual, expected);
    }

    Program BuildConditionalProgram() {
      std::vector<BaseFact> rhs = {BaseFact(Predicate(
            "isEmployee", {"somePerson"}, kPositive))};
      Fact lhs(BaseFact(Predicate("canAccess", {"somePerson", "someFile"},
              kPositive)));
      return BuildSingleAssertionProgram(
          BuildSingleSaysAssertion(Principal("TestSpeaker"),
            Assertion(ConditionalAssertion(std::move(lhs), std::move(rhs)))));
    }

    TEST(EmitterTestSuite, ConditionalProgramTest) {
      std::string expected = 
R"(says_canAccess(TestSpeaker, somePerson, someFile) :- says_isEmployee(TestSpeaker, somePerson).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canAccess(x0: symbol, x1: symbol, x2: symbol)
.decl says_isEmployee(x0: symbol, x1: symbol))";
      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildConditionalProgram()));
      EXPECT_EQ(actual, expected);
    }

    Program BuildMultiAssertionProgram() {
      // Conditional Assertion
      std::vector<BaseFact> rhs = {BaseFact(Predicate(
            "isEmployee", {"somePerson"}, kPositive))};
      Fact lhs(BaseFact(Predicate("canAccess", {"somePerson", "someFile"},
              kPositive)));
      SaysAssertion condAssertion = BuildSingleSaysAssertion(
          Principal("TestPrincipal"),
          Assertion(ConditionalAssertion(std::move(lhs),
            std::move(rhs))));

      // Assertion stating the condition
      SaysAssertion predicateAssertion = BuildSingleSaysAssertion(
          Principal("TestPrincipal"),
          Assertion(Fact(BaseFact(
            Predicate("isEmployee", {"somePerson"},
                kPositive))))
      );

      // I would love to just write this:
      // return Program({std::move(condAssertion),
      //     std::move(predicateAssertion)}, {});
   
      std::vector<SaysAssertion> assertion_list = {};
      assertion_list.push_back(std::move(condAssertion));
      assertion_list.push_back(std::move(predicateAssertion));
      return Program(std::move(assertion_list), {});
    }

    TEST(EmitterTestSuite, MultiAssertionProgramTest) {
      std::string expected = R"(says_canAccess(TestPrincipal, somePerson, someFile) :- says_isEmployee(TestPrincipal, somePerson).
says_isEmployee(TestPrincipal, somePerson).
grounded_dummy(dummy_var).

.decl grounded_dummy(x0: symbol)
.decl says_canAccess(x0: symbol, x1: symbol, x2: symbol)
.decl says_isEmployee(x0: symbol, x1: symbol))";
      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildMultiAssertionProgram()));
      EXPECT_EQ(actual, expected);
    }

    Program BuildQueryProgram() {
      Query testQuery("theTestQuery", Principal("TestSpeaker"),
          Fact(BaseFact(Predicate("anything", {"atAll"}, kPositive))));
      std::vector<Query> query_list = {};
      query_list.push_back(std::move(testQuery));
      return Program({}, std::move(query_list));
    }

    TEST(EmitterTestSuite, QueryTestProgram) {
      std::string expected =
R"(theTestQuery(dummy_var) :- says_anything(TestSpeaker, atAll), grounded_dummy(dummy_var).
grounded_dummy(dummy_var).
.output theTestQuery

.decl grounded_dummy(x0: symbol)
.decl says_anything(x0: symbol, x1: symbol)
.decl theTestQuery(x0: symbol))";

// R"(theTestQuery(dummy_var) :- says_anything(TestSpeaker, atAll), grounded_dummy(dummy_var).
// grounded_dummy(dummy_var).
// .output theTestQuery
// 
// .decl theTestQuery(x0: symbol)
// .decl says_anything(x0: symbol, x1: symbol)
// .decl grounded_dummy(x0: symbol))";

      std::string actual = SouffleEmitter::EmitProgram(
          LoweringToDatalogPass::Lower(BuildQueryProgram()));
      EXPECT_EQ(actual, expected);
    }

}
