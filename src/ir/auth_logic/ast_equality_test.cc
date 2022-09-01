//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

TEST(AstEqualityTestSuite, Test1) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(p1 == p2);
}

TEST(AstEqualityTestSuite, Test2) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("food", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test3) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"baz", "bar"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test4) {
  datalog::Predicate p1("foo", {"barrrrr", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test5) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "bas"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test6) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kNegated);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test7) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz", "beef"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, PrinTest) {
  Principal p1("A");
  Principal p2("A");
  Principal p3("B");
  EXPECT_TRUE(p1 == p1);
  EXPECT_TRUE(p1 == p2);
  EXPECT_TRUE(p2 == p1);
  EXPECT_TRUE(!(p1 == p3));
  EXPECT_TRUE(p1 != p3);
}

TEST(AstEqualityTest, AttributeTest) {
  Principal prin1("A");
  Principal prin2("B");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate pred2("foooo", {"bar", "baz"}, datalog::kPositive);
  Attribute attr1(prin1, pred1);
  Attribute attr2(prin1, pred1);
  Attribute attr3(prin2, pred1);
  Attribute attr4(prin1, pred2);
  Attribute attr5(prin2, pred2);
  EXPECT_TRUE(attr1 == attr1);
  EXPECT_TRUE(attr1 == attr2);
  EXPECT_TRUE(attr2 == attr1);
  EXPECT_TRUE(!(attr1 == attr3));
  EXPECT_TRUE(!(attr1 == attr4));
  EXPECT_TRUE(!(attr1 == attr5));
  EXPECT_TRUE(attr1 != attr3);
  EXPECT_TRUE(attr1 != attr3);
  EXPECT_TRUE(attr1 != attr3);
}

TEST(AstEqualityTest, CanActAsTest) {
  Principal prin1("A");
  Principal prin2("B");
  Principal prin3("C");
  CanActAs canActAs1(prin1, prin2);
  CanActAs canActAs2(prin1, prin2);
  CanActAs canActAs3(prin1, prin3);
  CanActAs canActAs4(prin2, prin3);
  EXPECT_TRUE(canActAs1 == canActAs1);
  EXPECT_TRUE(canActAs1 == canActAs2);
  EXPECT_TRUE(canActAs2 == canActAs1);
  EXPECT_TRUE(!(canActAs1 == canActAs3));
  EXPECT_TRUE(!(canActAs1 == canActAs4));
  EXPECT_TRUE(canActAs1 != canActAs3);
  EXPECT_TRUE(canActAs1 != canActAs4);
}

TEST(AstEqualityTest, BaseFactTest) {
  Principal prin1("A");
  Principal prin2("B");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate pred2("foooo", {"bar", "baz"}, datalog::kPositive);
  Attribute attr1(prin1, pred1);
  Attribute attr2(prin2, pred2);
  CanActAs canActAs1(prin1, prin1);
  CanActAs canActAs2(prin1, prin2);
  BaseFact baseFact1(pred1);
  BaseFact baseFact2(pred1);
  BaseFact baseFact3(pred2);
  BaseFact baseFact4(attr1);
  BaseFact baseFact5(attr1);
  BaseFact baseFact6(attr2);
  BaseFact baseFact7(canActAs1);
  BaseFact baseFact8(canActAs1);
  BaseFact baseFact9(canActAs2);
  EXPECT_TRUE(baseFact1 == baseFact1);
  EXPECT_TRUE(baseFact1 == baseFact2);
  EXPECT_TRUE(baseFact4 == baseFact4);
  EXPECT_TRUE(baseFact4 == baseFact5);
  EXPECT_TRUE(baseFact7 == baseFact7);
  EXPECT_TRUE(baseFact7 == baseFact8);
  EXPECT_TRUE(!(baseFact1 == baseFact3));
  EXPECT_TRUE(!(baseFact1 == baseFact4));
  EXPECT_TRUE(!(baseFact1 == baseFact7));
  EXPECT_TRUE(!(baseFact4 == baseFact6));
  EXPECT_TRUE(!(baseFact4 == baseFact1));
  EXPECT_TRUE(!(baseFact4 == baseFact7));
  EXPECT_TRUE(baseFact1 != baseFact3);
  EXPECT_TRUE(baseFact1 != baseFact4);
  EXPECT_TRUE(baseFact1 != baseFact7);
  EXPECT_TRUE(baseFact4 != baseFact6);
  EXPECT_TRUE(baseFact4 != baseFact1);
  EXPECT_TRUE(baseFact4 != baseFact7);
}

TEST(AstEqualityTest, FactTest) {
  Principal prin1("A");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate pred2("foooo", {"bar", "baz"}, datalog::kPositive);
  BaseFact baseFact1(pred1);
  BaseFact baseFact2(pred2);
  Fact fact1({}, baseFact1);
  Fact fact2({}, baseFact1);
  Fact fact3({prin1}, baseFact1);
  Fact fact4({}, baseFact2);
  Fact fact5({prin1}, baseFact2);
  EXPECT_TRUE(fact1 == fact1);
  EXPECT_TRUE(fact1 == fact2);
  EXPECT_TRUE(!(fact1 == fact3));
  EXPECT_TRUE(!(fact1 == fact4));
  EXPECT_TRUE(!(fact1 == fact5));
  EXPECT_TRUE(fact1 != fact3);
  EXPECT_TRUE(fact1 != fact4);
  EXPECT_TRUE(fact1 != fact5);
}

TEST(AstEqualityTest, ConditionalAssertion) {
  Principal prin1("A");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  BaseFact baseFact1(pred1);
  Fact fact1({}, baseFact1);
  ConditionalAssertion condAssertion1(fact1, {});
  ConditionalAssertion condAssertion2(fact1, {});
  ConditionalAssertion condAssertion3(fact1, {baseFact1});
  ConditionalAssertion condAssertion4(fact1, {baseFact1, baseFact1});
  EXPECT_TRUE(condAssertion1 == condAssertion1);
  EXPECT_TRUE(condAssertion1 == condAssertion2);
  EXPECT_TRUE(!(condAssertion1 == condAssertion3));
  EXPECT_TRUE(!(condAssertion1 == condAssertion4));
  EXPECT_TRUE(condAssertion1 != condAssertion3);
  EXPECT_TRUE(condAssertion1 != condAssertion4);
}

TEST(AstEqualityTest, Assertion) {
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  Fact fact1({}, BaseFact(pred1));
  ConditionalAssertion condAssertion1(fact1, {});
  Assertion assertion1(fact1);
  Assertion assertion2(fact1);
  Assertion assertion3(condAssertion1);
  Assertion assertion4(condAssertion1);
  EXPECT_TRUE(assertion1 == assertion1);
  EXPECT_TRUE(assertion1 == assertion2);
  EXPECT_TRUE(assertion3 == assertion3);
  EXPECT_TRUE(assertion3 == assertion4);
  EXPECT_TRUE(!(assertion1 == assertion3));
  EXPECT_TRUE(!(assertion4 == assertion2));
  EXPECT_TRUE(assertion1 != assertion3);
  EXPECT_TRUE(assertion4 != assertion2);
}

TEST(AstEqualityTest, SaysAssertion) {
  Principal prin1("A");
  Principal prin2("B");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  Fact fact1({}, BaseFact(pred1));
  Assertion assertion1(fact1);
  SaysAssertion saysAssertion1(prin1, {assertion1});
  SaysAssertion saysAssertion2(prin1, {assertion1});
  SaysAssertion saysAssertion3(prin2, {assertion1});
  SaysAssertion saysAssertion4(prin1, {assertion1, assertion1});
  SaysAssertion saysAssertion5(prin2, {assertion1, assertion1});
  EXPECT_TRUE(saysAssertion1 == saysAssertion1);
  EXPECT_TRUE(saysAssertion1 == saysAssertion2);
  EXPECT_TRUE(!(saysAssertion1 == saysAssertion3));
  EXPECT_TRUE(!(saysAssertion1 == saysAssertion4));
  EXPECT_TRUE(!(saysAssertion1 == saysAssertion5));
  EXPECT_TRUE(saysAssertion1 != saysAssertion3);
  EXPECT_TRUE(saysAssertion1 != saysAssertion4);
  EXPECT_TRUE(saysAssertion1 != saysAssertion5);
}

TEST(AstEqualityTest, Query) {
  Principal prin1("A");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate pred2("foo", {"bar", "baz"}, datalog::kPositive);
  Fact fact1({}, BaseFact(pred1));
  Fact fact2({}, BaseFact(pred2));
  Query query1("q", prin1, fact1);
  Query query2("q", prin1, fact1);
  Query query3("qqq", prin1, fact2);
  EXPECT_TRUE(query1 == query1);
  EXPECT_TRUE(query1 == query2);
  EXPECT_TRUE(!(query1 == query3));
  EXPECT_TRUE(query1 != query3);
}

TEST(AstEqualityTest, Program) {
  Principal prin1("A");
  datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
  Fact fact1({}, BaseFact(pred1));
  Assertion assertion1(fact1);
  SaysAssertion saysAssertion1(prin1, {assertion1});
  datalog::RelationDeclaration relationDeclaration1("rel1", false, {});
  Query query1("q", prin1, fact1);
  Program program1({relationDeclaration1}, {saysAssertion1}, {query1});
  Program program2({relationDeclaration1}, {saysAssertion1}, {query1});
  Program program3({}, {}, {});
  Program program4({relationDeclaration1}, {saysAssertion1}, {query1, query1});
  Program program5({relationDeclaration1}, {}, {query1, query1});
  EXPECT_TRUE(program1 == program1);
  EXPECT_TRUE(program1 == program2);
  EXPECT_TRUE(!(program1 == program3));
  EXPECT_TRUE(!(program1 == program4));
  EXPECT_TRUE(!(program1 == program5));
  EXPECT_TRUE(program1 != program3);
  EXPECT_TRUE(program1 != program4);
  EXPECT_TRUE(program1 != program5);
}

}  // namespace raksha::ir::auth_logic
