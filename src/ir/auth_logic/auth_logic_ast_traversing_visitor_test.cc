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
#include <iostream>

namespace raksha::ir::auth_logic {
namespace {

// A visitor that makes a set of all the names of principals in the program
class PrincipalNameCollectorVisitor
    : public AuthLogicAstTraversingVisitor<PrincipalNameCollectorVisitor,
                                           absl::flat_hash_set<std::string>> {
 public:
  absl::flat_hash_set<std::string> GetDefaultValue() override { return {}; }

  absl::flat_hash_set<std::string> CombineResult (
      absl::flat_hash_set<std::string> acc,
      absl::flat_hash_set<std::string> child_result) override {
    acc.merge(std::move(child_result));
    return acc;
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
    if (pre_visits_) {
      std::cout << "pre principal " << std::addressof(prin) << std::endl;
      nodes_.push_back(std::addressof(prin));
    }
    return Unit();
  }
  Unit PostVisit(const Principal& prin, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(prin));
    return result;
  }

  Unit PreVisit(const Attribute& attrib) override {
    if (pre_visits_) nodes_.push_back(std::addressof(attrib));
    return Unit();
  }
  Unit PostVisit(const Attribute& attrib, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(attrib));
    return result;
  }

  Unit PreVisit(const CanActAs& canActAs) override {
    if (pre_visits_) nodes_.push_back(std::addressof(canActAs));
    return Unit();
  }
  Unit PostVisit(const CanActAs& canActAs, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(canActAs));
    return result;
  }

  Unit PreVisit(const BaseFact& baseFact) override {
    if (pre_visits_) {
      std::cout << "pre baseFact " << std::addressof(baseFact)
        << std::endl;
      nodes_.push_back(std::addressof(baseFact));
    }
    return Unit();
  }
  Unit PostVisit(const BaseFact& baseFact, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(baseFact));
    return result;
  }

  Unit PreVisit(const Fact& fact) override {
    if (pre_visits_) {
      std::cout << "pre Fact " << std::addressof(fact)
        << std::endl;
      nodes_.push_back(std::addressof(fact));
    }
    return Unit();
  }
  Unit PostVisit(const Fact& fact, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(fact));
    return result;
  }

  Unit PreVisit(const ConditionalAssertion& condAssertion) override {
    if (pre_visits_) nodes_.push_back(std::addressof(condAssertion));
    return Unit();
  }
  Unit PostVisit(const ConditionalAssertion& condAssertion, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(condAssertion));
    return result;
  }

  Unit PreVisit(const Assertion& assertion) override {
    if (pre_visits_) nodes_.push_back(std::addressof(assertion));
    return Unit();
  }
  Unit PostVisit(const Assertion& assertion, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(assertion));
    return result;
  }

  Unit PreVisit(const SaysAssertion& saysAssertion) override {
    if (pre_visits_) nodes_.push_back(std::addressof(saysAssertion));
    return Unit();
  }
  Unit PostVisit(const SaysAssertion& saysAssertion, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(saysAssertion));
    return result;
  }

  Unit PreVisit(const Query& query) override {
    if (pre_visits_) nodes_.push_back(std::addressof(query));
    return Unit();
  }
  Unit PostVisit(const Query& query, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(query));
    return result;
  }

  Unit PreVisit(const Program& program) override {
    if (pre_visits_) nodes_.push_back(std::addressof(program));
    return Unit();
  }
  Unit PostVisit(const Program& program, Unit result) override {
    if (post_visits_) nodes_.push_back(std::addressof(program));
    return result;
  }

  const std::vector<const void*>& nodes() const { return nodes_; }

 private:
  bool pre_visits_;
  bool post_visits_;
  std::vector<const void*> nodes_;
};

TEST(AuthLogicAstTraversingVisitorTest, SimpleTraversalTest) {
  Principal prinA = Principal("PrincipalA");
  Principal prinB = Principal("PrincipalB");
  Principal prinC = Principal("PrincipalC");

  datalog::Predicate pred1 = datalog::Predicate("pred1", {}, datalog::kPositive);
  datalog::Predicate pred2 = datalog::Predicate("pred2", {}, datalog::kPositive);

  BaseFact baseFact1 = BaseFact(pred1);
  BaseFact baseFact2 = BaseFact(pred2);
  
  Fact fact1 = Fact({}, baseFact1);
  Fact fact2 = Fact({}, baseFact2);

  Assertion ast1 = Assertion(fact1);

  TraversalOrderVisitor preorder_visitor(TraversalType::kPre);
  fact1.Accept(preorder_visitor);
  EXPECT_THAT(
    preorder_visitor.nodes(),
    testing::ElementsAre(
      std::addressof(fact1),
      std::addressof(fact1.base_fact()))
  );

}


}  // namespace
}  // namespace raksha::ir::auth_logic
