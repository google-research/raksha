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

class PrincipalEqTest
    : public testing::TestWithParam<std::tuple<
          std::pair<Principal, uint64_t>, std::pair<Principal, uint64_t>>> {};

TEST_P(PrincipalEqTest, PrincipalEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static Principal prinA("A");
static Principal prinB("B");
static std::pair<Principal, uint64_t> principalTestVals[] = {{prinA, 0},
                                                             {prinB, 1}};

INSTANTIATE_TEST_SUITE_P(
    PrincipalEqTest, PrincipalEqTest,
    testing::Combine(testing::ValuesIn(principalTestVals),
                     testing::ValuesIn(principalTestVals)));

class AttributeEqTest
    : public testing::TestWithParam<std::tuple<
          std::pair<Attribute, uint64_t>, std::pair<Attribute, uint64_t>>> {};

TEST_P(AttributeEqTest, AttributeEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static datalog::Predicate pred1("foo", {"bar", "baz"}, datalog::kPositive);
static datalog::Predicate pred2("foooo", {"bar", "baz"}, datalog::kPositive);
static datalog::Predicate pred3("foo", {"bar", "baz"}, datalog::kNegated);

static Attribute attr1(prinA, pred1);
static Attribute attr2(prinB, pred2);
static Attribute attr3(prinA, pred2);
static Attribute attr4(prinB, pred1);
static Attribute attr5(prinB, pred3);
static std::pair<Attribute, uint64_t> attributeTestVals[] = {
    {attr1, 0}, {attr2, 1}, {attr3, 2}, {attr4, 3}, {attr5, 4},
};

INSTANTIATE_TEST_SUITE_P(
    AttributeEqTest, AttributeEqTest,
    testing::Combine(testing::ValuesIn(attributeTestVals),
                     testing::ValuesIn(attributeTestVals)));

class CanActAsEqTest
    : public testing::TestWithParam<std::tuple<std::pair<CanActAs, uint64_t>,
                                               std::pair<CanActAs, uint64_t>>> {
};

TEST_P(CanActAsEqTest, CanActAsEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static Principal prinC("C");
CanActAs canActAs1(prinA, prinB);
CanActAs canActAs2(prinA, prinC);
CanActAs canActAs3(prinB, prinC);
static std::pair<CanActAs, uint64_t> canActAsTestVals[] = {
    {canActAs1, 0},
    {canActAs2, 1},
    {canActAs3, 2},
};

INSTANTIATE_TEST_SUITE_P(CanActAsEqTest, CanActAsEqTest,
                         testing::Combine(testing::ValuesIn(canActAsTestVals),
                                          testing::ValuesIn(canActAsTestVals)));

class BaseFactEqTest
    : public testing::TestWithParam<std::tuple<std::pair<BaseFact, uint64_t>,
                                               std::pair<BaseFact, uint64_t>>> {
};

TEST_P(BaseFactEqTest, BaseFactEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static BaseFact baseFact1(pred1);
static BaseFact baseFact2(pred2);
static BaseFact baseFact3(attr1);
static BaseFact baseFact4(attr2);
static BaseFact baseFact5(canActAs1);
static BaseFact baseFact6(canActAs2);
static std::pair<BaseFact, uint64_t> baseFactTestVals[] = {
    {baseFact1, 0}, {baseFact2, 1}, {baseFact3, 2},
    {baseFact4, 3}, {baseFact5, 4}, {baseFact6, 5},
};

INSTANTIATE_TEST_SUITE_P(BaseFactEqTest, BaseFactEqTest,
                         testing::Combine(testing::ValuesIn(baseFactTestVals),
                                          testing::ValuesIn(baseFactTestVals)));

class FactEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<Fact, uint64_t>, std::pair<Fact, uint64_t>>> {};

TEST_P(FactEqTest, FactEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}
static Fact fact1({}, baseFact1);
static Fact fact2({prinA}, baseFact1);
static Fact fact3({}, baseFact2);
static Fact fact4({prinB}, baseFact2);
static std::pair<Fact, uint64_t> factTestVals[] = {
    {fact1, 0},
    {fact2, 1},
    {fact3, 2},
    {fact4, 3},
};

INSTANTIATE_TEST_SUITE_P(FactEqTest, FactEqTest,
                         testing::Combine(testing::ValuesIn(factTestVals),
                                          testing::ValuesIn(factTestVals)));

class ConditionalAssertionEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<ConditionalAssertion, uint64_t>,
                     std::pair<ConditionalAssertion, uint64_t>>> {};

TEST_P(ConditionalAssertionEqTest, ConditionalAssertionEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static ConditionalAssertion condAssertion1(fact1, {});
static ConditionalAssertion condAssertion2(fact1, {baseFact1});
static ConditionalAssertion condAssertion3(fact1, {baseFact1, baseFact1});
static std::pair<ConditionalAssertion, uint64_t> condAsstTestVals[] = {
    {condAssertion1, 0}, {condAssertion2, 1}, {condAssertion3, 2}};

INSTANTIATE_TEST_SUITE_P(ConditionalAssertionEqTest, ConditionalAssertionEqTest,
                         testing::Combine(testing::ValuesIn(condAsstTestVals),
                                          testing::ValuesIn(condAsstTestVals)));

class AssertionEqTest
    : public testing::TestWithParam<std::tuple<
          std::pair<Assertion, uint64_t>, std::pair<Assertion, uint64_t>>> {};

TEST_P(AssertionEqTest, AssertionEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

Assertion assertion1(fact1);
Assertion assertion2(condAssertion1);
static std::pair<Assertion, uint64_t> asstTestVals[] = {
    {assertion1, 0},
    {assertion2, 1},
};

INSTANTIATE_TEST_SUITE_P(AssertionEqTest, AssertionEqTest,
                         testing::Combine(testing::ValuesIn(asstTestVals),
                                          testing::ValuesIn(asstTestVals)));

class SaysAssertionEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<SaysAssertion, uint64_t>,
                     std::pair<SaysAssertion, uint64_t>>> {};

TEST_P(SaysAssertionEqTest, SaysAssertionEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static SaysAssertion saysAssertion1(prinA, {assertion1});
static SaysAssertion saysAssertion2(prinB, {assertion1});
static SaysAssertion saysAssertion3(prinA, {assertion1, assertion1});
static SaysAssertion saysAssertion4(prinB, {assertion1, assertion1});
static std::pair<SaysAssertion, uint64_t> saysAsstTestVals[] = {
    {saysAssertion1, 0},
    {saysAssertion2, 1},
    {saysAssertion3, 2},
    {saysAssertion4, 3},
};

INSTANTIATE_TEST_SUITE_P(SaysAssertionEqTest, SaysAssertionEqTest,
                         testing::Combine(testing::ValuesIn(saysAsstTestVals),
                                          testing::ValuesIn(saysAsstTestVals)));

class QueryEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<Query, uint64_t>, std::pair<Query, uint64_t>>> {
};

TEST_P(QueryEqTest, QueryEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static Query query1("q", prinA, fact1);
static Query query2("q", prinB, fact1);
static Query query3("qqq", prinA, fact2);
static std::pair<Query, uint64_t> queryTestVals[] = {
    {query1, 0},
    {query2, 1},
    {query3, 2},
};

INSTANTIATE_TEST_SUITE_P(QueryEqTest, QueryEqTest,
                         testing::Combine(testing::ValuesIn(queryTestVals),
                                          testing::ValuesIn(queryTestVals)));

class ProgramEqTest
    : public testing::TestWithParam<std::tuple<std::pair<Program, uint64_t>,
                                               std::pair<Program, uint64_t>>> {
};

TEST_P(ProgramEqTest, ProgramEqTest) {
  auto &[obj_and_eq_class1, obj_and_eq_class2] = GetParam();
  auto &[obj1, eq_class1] = obj_and_eq_class1;
  auto &[obj2, eq_class2] = obj_and_eq_class2;

  EXPECT_EQ(obj1 == obj2, eq_class1 == eq_class2);
  EXPECT_EQ(obj1 != obj2, eq_class1 != eq_class2);
}

static datalog::RelationDeclaration relationDeclaration1("rel1", false, {});
static Program program1({relationDeclaration1}, {saysAssertion1}, {query1});
static Program program2({}, {}, {});
static Program program3({relationDeclaration1}, {saysAssertion1},
                        {query1, query1});
static Program program4({relationDeclaration1}, {}, {query1, query1});
static std::pair<Program, uint64_t> progTestVals[] = {
    {program1, 0},
    {program2, 1},
    {program3, 2},
    {program4, 3},
};

INSTANTIATE_TEST_SUITE_P(ProgramEqTest, ProgramEqTest,
                         testing::Combine(testing::ValuesIn(progTestVals),
                                          testing::ValuesIn(progTestVals)));

}  // namespace raksha::ir::auth_logic
