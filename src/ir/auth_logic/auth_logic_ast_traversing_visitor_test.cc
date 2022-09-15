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

#include <iostream>

#include "absl/container/btree_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {
namespace {

// A visitor that makes a set of all the names of principals in the program
class PrincipalNameCollectorVisitor
    : public AuthLogicAstTraversingVisitor<PrincipalNameCollectorVisitor,
                                           absl::btree_set<std::string>> {
 public:
  absl::btree_set<std::string> GetDefaultValue() override { return {}; }

  absl::btree_set<std::string> CombineResult(
      absl::btree_set<std::string> acc,
      absl::btree_set<std::string> child_result) override {
    acc.merge(std::move(child_result));
    return acc;
  }

  absl::btree_set<std::string> PreVisit(
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
  const absl::btree_set<std::string> result =
      test_prog.Accept(collector_visitor);
  const absl::btree_set<std::string> expected = {"PrincipalA", "PrincipalB",
                                                     "PrincipalC"};
  EXPECT_EQ(result, expected);
}

enum class TraversalType { kPre = 0x1, kPost = 0x2, kBoth = 0x3 };
class TraversalOrderVisitor
    : public AuthLogicAstTraversingVisitor<TraversalOrderVisitor> {
 public:
  TraversalOrderVisitor(TraversalType traversal_type)
      : pre_visits_(traversal_type == TraversalType::kPre ||
                    traversal_type == TraversalType::kBoth),
        post_visits_(traversal_type == TraversalType::kPost ||
                     traversal_type == TraversalType::kBoth) {}

  Unit PreVisit(const Principal& prin) override {
    if (pre_visits_) nodes_.push_back(prin.ToString());
    return Unit();
  }
  Unit PostVisit(const Principal& prin, Unit result) override {
    if (post_visits_) nodes_.push_back(prin.ToString());
    return result;
  }

  Unit PreVisit(const Attribute& attrib) override {
    if (pre_visits_) nodes_.push_back(attrib.ToString());
    return Unit();
  }
  Unit PostVisit(const Attribute& attrib, Unit result) override {
    if (post_visits_) nodes_.push_back(attrib.ToString());
    return result;
  }

  Unit PreVisit(const CanActAs& canActAs) override {
    if (pre_visits_) nodes_.push_back(canActAs.ToString());
    return Unit();
  }
  Unit PostVisit(const CanActAs& canActAs, Unit result) override {
    if (post_visits_) nodes_.push_back(canActAs.ToString());
    return result;
  }

  Unit PreVisit(const BaseFact& baseFact) override {
    if (pre_visits_) nodes_.push_back(baseFact.ToString());
    return Unit();
  }
  Unit PostVisit(const BaseFact& baseFact, Unit result) override {
    if (post_visits_) nodes_.push_back(baseFact.ToString());
    return result;
  }

  Unit PreVisit(const Fact& fact) override {
    if (pre_visits_) nodes_.push_back(fact.ToString());
    return Unit();
  }
  Unit PostVisit(const Fact& fact, Unit result) override {
    if (post_visits_) nodes_.push_back(fact.ToString());
    return result;
  }

  Unit PreVisit(const ConditionalAssertion& condAssertion) override {
    if (pre_visits_) nodes_.push_back(condAssertion.ToString());
    return Unit();
  }
  Unit PostVisit(const ConditionalAssertion& condAssertion,
                 Unit result) override {
    if (post_visits_) nodes_.push_back(condAssertion.ToString());
    return result;
  }

  Unit PreVisit(const Assertion& assertion) override {
    if (pre_visits_) nodes_.push_back(assertion.ToString());
    return Unit();
  }
  Unit PostVisit(const Assertion& assertion, Unit result) override {
    if (post_visits_) nodes_.push_back(assertion.ToString());
    return result;
  }

  Unit PreVisit(const SaysAssertion& saysAssertion) override {
    if (pre_visits_) nodes_.push_back(saysAssertion.ToString());
    return Unit();
  }
  Unit PostVisit(const SaysAssertion& saysAssertion, Unit result) override {
    if (post_visits_) nodes_.push_back(saysAssertion.ToString());
    return result;
  }

  Unit PreVisit(const Query& query) override {
    if (pre_visits_) nodes_.push_back(query.ToString());
    return Unit();
  }
  Unit PostVisit(const Query& query, Unit result) override {
    if (post_visits_) nodes_.push_back(query.ToString());
    return result;
  }

  Unit PreVisit(const Program& program) override {
    if (pre_visits_) nodes_.push_back(program.ToString());
    return Unit();
  }
  Unit PostVisit(const Program& program, Unit result) override {
    if (post_visits_) nodes_.push_back(program.ToString());
    return result;
  }

  const std::vector<std::string>& nodes() const { return nodes_; }

 private:
  bool pre_visits_;
  bool post_visits_;
  std::vector<std::string> nodes_;
};

TEST(AuthLogicAstTraversingVisitorTest, SimpleTraversalTest) {
  Principal prinA("PrincipalA");
  Principal prinB("PrincipalB");
  Principal prinC("PrincipalC");

  datalog::Predicate pred1("pred1", {}, datalog::kPositive);
  datalog::Predicate pred2("pred2", {}, datalog::kPositive);

  BaseFact baseFact1(pred1);
  BaseFact baseFact2(pred2);

  Fact fact1({}, baseFact1);
  Fact fact2({prinB}, baseFact2);

  Assertion assertion1(fact1);
  Assertion assertion2(fact2);

  SaysAssertion saysAssertion1(prinA, {assertion1});
  SaysAssertion saysAssertion2(prinC, {assertion1, assertion2});

  Query query1("query1", prinA, fact1);
  Query query2("query2", prinB, fact2);

  Program program1({}, {saysAssertion1, saysAssertion2}, {query1, query2});

  TraversalOrderVisitor preorder_visitor(TraversalType::kPre);
  program1.Accept(preorder_visitor);
  EXPECT_THAT(
      preorder_visitor.nodes(),
      testing::ElementsAre(
          program1.ToString(), saysAssertion1.ToString(),
          prinA.ToString(), assertion1.ToString(), fact1.ToString(),
          baseFact1.ToString(), saysAssertion2.ToString(),
          prinC.ToString(), assertion1.ToString(), fact1.ToString(),
          baseFact1.ToString(), assertion2.ToString(), fact2.ToString(),
          prinB.ToString(), baseFact2.ToString(), query1.ToString(),
          prinA.ToString(), fact1.ToString(), baseFact1.ToString(),
          query2.ToString(), prinB.ToString(), fact2.ToString(),
          prinB.ToString(), baseFact2.ToString()));

  TraversalOrderVisitor postorder_visitor(TraversalType::kPost);
  program1.Accept(postorder_visitor);
  EXPECT_THAT(
      postorder_visitor.nodes(),
      testing::ElementsAre(
          prinA.ToString(), baseFact1.ToString(), fact1.ToString(),
          assertion1.ToString(), saysAssertion1.ToString(),
          prinC.ToString(), baseFact1.ToString(), fact1.ToString(),
          assertion1.ToString(), prinB.ToString(), baseFact2.ToString(),
          fact2.ToString(), assertion2.ToString(),
          saysAssertion2.ToString(), prinA.ToString(),
          baseFact1.ToString(), fact1.ToString(), query1.ToString(),
          prinB.ToString(), prinB.ToString(), baseFact2.ToString(),
          fact2.ToString(), query2.ToString(), program1.ToString()));

  // The bits of syntax not in program1
  Attribute attribute1(prinC, pred2);
  CanActAs canActAs1(prinA, prinB);
  BaseFact baseFact3(attribute1);
  BaseFact baseFact4(canActAs1);
  Fact fact3({}, baseFact3);
  ConditionalAssertion conditionalAssertion1(fact3, {baseFact4});

  TraversalOrderVisitor preorder_visitor2(TraversalType::kPre);
  conditionalAssertion1.Accept(preorder_visitor2);
  EXPECT_THAT(
      preorder_visitor2.nodes(),
      testing::ElementsAre(conditionalAssertion1.ToString(),
                           fact3.ToString(), baseFact3.ToString(),
                           attribute1.ToString(), prinC.ToString(),
                           baseFact4.ToString(), canActAs1.ToString(),
                           prinA.ToString(), prinB.ToString()));

  TraversalOrderVisitor postorder_visitor2(TraversalType::kPost);
  conditionalAssertion1.Accept(postorder_visitor2);
  EXPECT_THAT(
      postorder_visitor2.nodes(),
      testing::ElementsAre(prinC.ToString(), attribute1.ToString(),
                           baseFact3.ToString(), fact3.ToString(),
                           prinA.ToString(), prinB.ToString(),
                           canActAs1.ToString(), baseFact4.ToString(),
                           conditionalAssertion1.ToString()));

  TraversalOrderVisitor both_order_visitor(TraversalType::kBoth);
  canActAs1.Accept(both_order_visitor);
  EXPECT_THAT(both_order_visitor.nodes(),
              testing::ElementsAre(canActAs1.ToString(), prinA.ToString(),
                                   prinA.ToString(), prinB.ToString(),
                                   prinB.ToString(), canActAs1.ToString()));
}

}  // namespace
}  // namespace raksha::ir::auth_logic
