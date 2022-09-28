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
#include "src/ir/auth_logic/ast_string.h"

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/ast_construction.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

static datalog::Predicate predicate1("predicate1", {"arg1", "arg2"},
                                     datalog::Sign::kPositive);
static datalog::Predicate predicate2("predicate2", {"arg1"},
                                     datalog::Sign::kNegated);
TEST(PredicateToStringTest, SimplePredicateTest) {
  EXPECT_EQ(ToString(predicate1), "predicate1(arg1, arg2)");
  EXPECT_EQ(ToString(predicate2), "!predicate2(arg1)");
}

static datalog::ArgumentType numberType(datalog::ArgumentType::Kind::kNumber,
                                        "");
static datalog::ArgumentType principalType(
    datalog::ArgumentType::Kind::kPrincipal, "");
static datalog::ArgumentType customType(datalog::ArgumentType::Kind::kCustom,
                                        "someProgrammerType");
TEST(ArgumentTypeToStringTest, SimpleArgumentTypeTest) {
  EXPECT_EQ(ToString(numberType), "NumberType");
  EXPECT_EQ(ToString(principalType), "PrincipalType");
  EXPECT_EQ(ToString(customType), "someProgrammerType::CustomType");
}

static datalog::Argument argument1("arg1", numberType);
static datalog::Argument argument2("arg2", principalType);
static datalog::Argument argument3("arg3", customType);
TEST(ArgumentToStringTest, SimpleArgumentTest) {
  EXPECT_EQ(ToString(argument1), "arg1 : NumberType");
  EXPECT_EQ(ToString(argument2), "arg2 : PrincipalType");
  EXPECT_EQ(ToString(argument3), "arg3 : someProgrammerType::CustomType");
}

static datalog::RelationDeclaration relationDeclaration1("rel1", false,
                                                         {argument1,
                                                          argument2});
static datalog::RelationDeclaration relationDeclaration2("rel2", true,
                                                         {argument3});
TEST(RelationDeclarationToStringTest, SimpleDeclarationTest) {
  EXPECT_EQ(ToString(relationDeclaration1),
            ".decl rel1(arg1 : NumberType, arg2 : PrincipalType)");
  EXPECT_EQ(ToString(relationDeclaration2),
            ".decl attribute rel2(arg3 : someProgrammerType::CustomType)");
}

static Principal principal1("principal1");

TEST(PrincipalToStringTest, SimplePrincipalTest) {
  EXPECT_EQ(ToString(principal1), "principal1");
}

static Principal principal2("principal2");
static Attribute attribute1(principal1, predicate1);

TEST(AttributeToStringTest, SimpleAttributeTest) {
  EXPECT_EQ(ToString(attribute1), "principal1 predicate1(arg1, arg2)");
}

static CanActAs canActAs1(principal1, principal2);

TEST(CanActAsToStringTest, SimpleCanActAsTest) {
  EXPECT_EQ(ToString(canActAs1), "principal1 canActAs principal2");
}

static BaseFact baseFact1(predicate1);
static BaseFact baseFact2(attribute1);
static BaseFact baseFact3(canActAs1);
TEST(BaseFactToStringTest, SimpleBaseFactTest) {
  EXPECT_EQ(ToString(baseFact1), "predicate1(arg1, arg2)");
  EXPECT_EQ(ToString(baseFact2), "principal1 predicate1(arg1, arg2)");
  EXPECT_EQ(ToString(baseFact3), "principal1 canActAs principal2");
}

static Fact fact1({}, baseFact1);
static Fact fact2({principal1}, baseFact1);
TEST(FactToStringTest, SimpleFactTest) {
  EXPECT_EQ(ToString(fact1), "predicate1(arg1, arg2)");
  EXPECT_EQ(ToString(fact2), "principal1 canSay predicate1(arg1, arg2)");
}

static ConditionalAssertion condAssertion1(fact1, {baseFact1});
static ConditionalAssertion condAssertion2(fact1, {baseFact1, baseFact2});
TEST(ConditionalAssertionToStringTest, SimpleCondAssertionTest) {
  EXPECT_EQ(ToString(condAssertion1),
            "predicate1(arg1, arg2) :- "
            "predicate1(arg1, arg2)");
  EXPECT_EQ(ToString(condAssertion2),
            "predicate1(arg1, arg2) :- predicate1(arg1, "
            "arg2), principal1 predicate1(arg1, arg2)");
}

static Assertion assertion1(fact1);
static Assertion assertion2(condAssertion1);
TEST(AssertionToStringTest, SimpleAssertionTest) {
  EXPECT_EQ(ToString(assertion1), "predicate1(arg1, arg2).");
  EXPECT_EQ(ToString(assertion2),
            "predicate1(arg1, arg2) :- "
            "predicate1(arg1, arg2).");
}

static SaysAssertion saysAssertion1(principal1, {assertion1});
static SaysAssertion saysAssertion2(principal2, {assertion1, assertion2});
TEST(SaysAssertionToStringTest, SimpleSaysAssertionTest) {
  EXPECT_EQ(ToString(saysAssertion1),
            "principal1 says {\npredicate1(arg1, arg2).}");
  EXPECT_EQ(ToString(saysAssertion2),
            "principal2 says {\npredicate1(arg1, "
            "arg2).\npredicate1(arg1, arg2) :- "
            "predicate1(arg1, arg2).}");
}

static Query query1("query1", principal1, fact1);
static Query query2("query2", principal2, fact1);
TEST(QueryToStringTest, SimpleQueryTest) {
  EXPECT_EQ(ToString(query1),
            "Query(query1 = principal1 says predicate1(arg1, "
            "arg2)?)");
  EXPECT_EQ(ToString(query2),
            "Query(query2 = principal2 says predicate1(arg1, "
            "arg2)?)");
}

static Program program1({relationDeclaration1}, {saysAssertion1}, {query1});
TEST(ProgramToStringTest, SimpleProgramTest) {
  EXPECT_EQ(
      ToString(program1),
      "Program(\n.decl rel1(arg1 : NumberType, arg2 : PrincipalType)principal1 "
      "says {\npredicate1(arg1, arg2).}Query(query1 = "
      "principal1 says predicate1(arg1, arg2)?))");
}

}  // namespace raksha::ir::auth_logic
