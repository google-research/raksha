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

// This file describes AST nodes for authorization logic

#ifndef SRC_IR_AUTH_LOGIC_AST_H_
#define SRC_IR_AUTH_LOGIC_AST_H_

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "absl/hash/hash.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/auth_logic/auth_logic_ast_visitor.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

class Principal {
 public:
  explicit Principal(std::string name) : name_(std::move(name)) {}
  const std::string& name() const { return name_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const { return name_; }

 private:
  std::string name_;
};

// Attribute corresponds to an attribute of a principal with the form
// <Principal> <pred_name>(arg_1, ..., arg_n)
class Attribute {
 public:
  explicit Attribute(Principal principal, datalog::Predicate predicate)
      : principal_(principal), predicate_(predicate) {}
  const Principal& principal() const { return principal_; }
  const datalog::Predicate& predicate() const { return predicate_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    return absl::StrCat(principal_.name(), predicate_.DebugPrint());
  }

 private:
  Principal principal_;
  datalog::Predicate predicate_;
};

// CanActAs corresponds to a canActAs expression of the form
// <principal_1> canActAs <principal_2>
class CanActAs {
 public:
  explicit CanActAs(Principal left_principal, Principal right_principal)
      : left_principal_(std::move(left_principal)),
        right_principal_(std::move(right_principal)) {}
  const Principal& left_principal() const { return left_principal_; }
  const Principal& right_principal() const { return right_principal_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    return absl::StrCat(left_principal_.DebugPrint(), " canActAs ",
                        right_principal_.DebugPrint());
  }

 private:
  Principal left_principal_;
  Principal right_principal_;
};

// BaseFact corresponds to a base-case fact that cannot recursively include
// other facts through "canSay". "Fact" includes the recursive case. It is
// necessary that BaseFact is a separate class from Fact because the RHS of
// conditional assertions can only contain BaseFacts.
// Base facts have three forms:
//  - A predicate
//  - An attribute
//  - A canActAs expression
class BaseFact {
 public:
  // BaseFactVariantType gives the different forms of BaseFacts. Client code
  // should use this type to traverse these forms. This type may be changed in
  // the future.
  using BaseFactVariantType =
      std::variant<datalog::Predicate, Attribute, CanActAs>;
  explicit BaseFact(BaseFactVariantType value) : value_(std::move(value)){};
  const BaseFactVariantType& GetValue() const { return value_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    return absl::StrCat(
        "BaseFact(",
        std::visit([](auto& obj) { return obj.DebugPrint(); }, this->value_),
        ")");
  }

 private:
  BaseFactVariantType value_;
};

// Fact corresponds to either a base fact or a an expression of the form
// <principal> canSay <Fact>
class Fact {
 public:
  Fact(std::forward_list<Principal> delegation_chain, BaseFact base_fact)
      : delegation_chain_(std::move(delegation_chain)),
        base_fact_(std::move(base_fact)) {}

  const std::forward_list<Principal>& delegation_chain() const {
    return delegation_chain_;
  }

  const BaseFact& base_fact() const { return base_fact_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    std::vector<std::string> delegations;
    for (const Principal& delegatee : delegation_chain_) {
      delegations.push_back(delegatee.DebugPrint());
    }
    return absl::StrCat("deleg: { ", absl::StrJoin(delegations, ", "), " }",
                        base_fact_.DebugPrint());
  }

 private:
  std::forward_list<Principal> delegation_chain_;
  BaseFact base_fact_;
};

// ConditionalAssertion the particular form of assertion that can have
// conditions which is:
// <fact> :- <flat_fact_1> ... <flat_fact_n>
class ConditionalAssertion {
 public:
  explicit ConditionalAssertion(Fact lhs, std::vector<BaseFact> rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  const Fact& lhs() const { return lhs_; }
  const std::vector<BaseFact>& rhs() const { return rhs_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    std::vector<std::string> rhs_strings;
    for (const BaseFact& base_fact : rhs_) {
      rhs_strings.push_back(base_fact.DebugPrint());
    }
    return absl::StrCat(lhs_.DebugPrint(), ":-",
                        absl::StrJoin(rhs_strings, ", "));
  }

 private:
  Fact lhs_;
  std::vector<BaseFact> rhs_;
};

// Assertions can have two forms:
//  - Facts
//  - Conditional assertions
class Assertion {
 public:
  // AssertionVariantType gives the different forms of Facts. Client code
  // should use this type to traverse these forms. This type may be changed in
  // the future.
  using AssertionVariantType = std::variant<Fact, ConditionalAssertion>;
  explicit Assertion(AssertionVariantType value) : value_(std::move(value)) {}
  const AssertionVariantType& GetValue() const { return value_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    return absl::StrCat(
        "Assertion(",
        std::visit([](auto& obj) { return obj.DebugPrint(); }, this->value_),
        ")");
  }

 private:
  AssertionVariantType value_;
};

// SaysAssertion prepends an assertion with "<principal> says"
class SaysAssertion {
 public:
  explicit SaysAssertion(Principal principal, std::vector<Assertion> assertions)
      : principal_(std::move(principal)), assertions_(std::move(assertions)){};
  const Principal& principal() const { return principal_; }
  const std::vector<Assertion>& assertions() const { return assertions_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    std::vector<std::string> assertion_strings;
    for (const Assertion& assertion : assertions_) {
      assertion_strings.push_back(assertion.DebugPrint());
    }
    return absl::StrCat(principal_.DebugPrint(), "says {\n",
                        absl::StrJoin(assertion_strings, "\n"), "}");
  }

 private:
  Principal principal_;
  std::vector<Assertion> assertions_;
};

// A query which can be used to test whether "<principal> says <fact>" is true
// given the rules and base facts.
class Query {
 public:
  explicit Query(std::string name, Principal principal, Fact fact)
      : name_(std::move(name)),
        principal_(std::move(principal)),
        fact_(std::move(fact)) {}
  const std::string& name() const { return name_; }
  const Principal& principal() const { return principal_; }
  const Fact& fact() const { return fact_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    return absl::StrCat("Query(", name_, principal_.DebugPrint(),
                        fact_.DebugPrint(), ")");
  }

 private:
  std::string name_;
  Principal principal_;
  Fact fact_;
};

// This is a top-level program.
class Program {
 public:
  explicit Program(
      std::vector<datalog::RelationDeclaration> relation_declarations,
      std::vector<SaysAssertion> says_assertions, std::vector<Query> queries)
      : relation_declarations_(std::move(relation_declarations)),
        says_assertions_(std::move(says_assertions)),
        queries_(std::move(queries)) {}

  const std::vector<datalog::RelationDeclaration>& relation_declarations()
      const {
    return relation_declarations_;
  }

  const std::vector<SaysAssertion>& says_assertions() const {
    return says_assertions_;
  }

  const std::vector<Query>& queries() const { return queries_; }

  template <typename Derived, typename Result>
  Result Accept(AuthLogicAstVisitor<Derived, Result, Mutable>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(
      AuthLogicAstVisitor<Derived, Result, Immutable>& visitor) const {
    return visitor.Visit(*this);
  }

  // A potentially ugly print of the state in this class
  // for debugging/testing only
  std::string DebugPrint() const {
    std::vector<std::string> relation_decl_strings;
    for (const datalog::RelationDeclaration& rel_decl :
         relation_declarations_) {
      relation_decl_strings.push_back(rel_decl.DebugPrint());
    }
    std::vector<std::string> says_assertion_strings;
    for (const SaysAssertion& says_assertion : says_assertions_) {
      says_assertion_strings.push_back(says_assertion.DebugPrint());
    }
    std::vector<std::string> query_strings;
    for (const Query& query : queries_) {
      query_strings.push_back(query.DebugPrint());
    }
    return absl::StrCat("Program(\n",
                        absl::StrJoin(relation_decl_strings, "\n"),
                        absl::StrJoin(says_assertion_strings, "\n"),
                        absl::StrJoin(query_strings, "\n"), ")");
  }

 private:
  std::vector<datalog::RelationDeclaration> relation_declarations_;
  std::vector<SaysAssertion> says_assertions_;
  std::vector<Query> queries_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
