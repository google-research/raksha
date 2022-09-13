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
#include "src/ir/ast_node.h"

namespace raksha::ir::auth_logic {

class Principal : public AstNode {
 public:
  explicit Principal(std::string name) : name_(std::move(name)) {}
  virtual ~Principal() = default;
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

  bool operator==(const Principal& rhs) const { return name_ == rhs.name(); }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Principal& rhsPrincipal = dynamic_cast<const Principal&>(rhs);
      return *this == rhsPrincipal ;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Principal& rhs) const { return !(*this == rhs); }

 private:
  std::string name_;
};

// Attribute corresponds to an attribute of a principal with the form
// <Principal> <pred_name>(arg_1, ..., arg_n)
class Attribute : public AstNode {
 public:
  explicit Attribute(Principal principal, datalog::Predicate predicate)
      : principal_(principal), predicate_(predicate) {}
  virtual ~Attribute() = default;
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

  bool operator==(const Attribute& rhs) const {
    return principal_ == rhs.principal() && predicate_ == rhs.predicate();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Attribute & rhsAttribute = dynamic_cast<const Attribute&>(rhs);
      return *this == rhsAttribute;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Attribute& rhs) const { return !(*this == rhs); }

 private:
  Principal principal_;
  datalog::Predicate predicate_;
};

// CanActAs corresponds to a canActAs expression of the form
// <principal_1> canActAs <principal_2>
class CanActAs : public AstNode {
 public:
  explicit CanActAs(Principal left_principal, Principal right_principal)
      : left_principal_(std::move(left_principal)),
        right_principal_(std::move(right_principal)) {}
  virtual ~CanActAs() = default;
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


  bool operator==(const CanActAs& rhs) const {
    return left_principal_ == rhs.left_principal() &&
           right_principal_ == rhs.right_principal();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const CanActAs & rhsCanActAs = dynamic_cast<const CanActAs&>(rhs);
      return *this == rhsCanActAs;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }
  bool operator!=(const CanActAs& rhs) const { return !(*this == rhs); }

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
class BaseFact : public AstNode {
 public:
  // BaseFactVariantType gives the different forms of BaseFacts. Client code
  // should use this type to traverse these forms. This type may be changed in
  // the future.
  using BaseFactVariantType =
      std::variant<datalog::Predicate, Attribute, CanActAs>;
  explicit BaseFact(BaseFactVariantType value) : value_(std::move(value)){};
  virtual ~BaseFact() = default;
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

  bool operator==(const BaseFact& rhs) const {
    return value_ == rhs.GetValue();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const BaseFact & rhsBaseFact = dynamic_cast<const BaseFact&>(rhs);
      return *this == rhsBaseFact;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }
  bool operator!=(const BaseFact& rhs) const { return !(*this == rhs); }

 private:
  BaseFactVariantType value_;
};

// Fact corresponds to either a base fact or a an expression of the form
// <principal> canSay <Fact>
class Fact : public AstNode {
 public:
  Fact(std::forward_list<Principal> delegation_chain, BaseFact base_fact)
      : delegation_chain_(std::move(delegation_chain)),
        base_fact_(std::move(base_fact)) {}
  virtual ~Fact() = default;

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

  bool operator==(const Fact& rhs) const {
    return delegation_chain_ == rhs.delegation_chain() &&
           base_fact_ == rhs.base_fact();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Fact & rhsFact = dynamic_cast<const Fact&>(rhs);
      return *this == rhsFact;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Fact& rhs) const { return !(*this == rhs); }

 private:
  std::forward_list<Principal> delegation_chain_;
  BaseFact base_fact_;
};

// ConditionalAssertion the particular form of assertion that can have
// conditions which is:
// <fact> :- <flat_fact_1> ... <flat_fact_n>
class ConditionalAssertion : public AstNode {
 public:
  explicit ConditionalAssertion(Fact lhs, std::vector<BaseFact> rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  virtual ~ConditionalAssertion() = default;
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

  bool operator==(const ConditionalAssertion& rhs) const {
    return lhs_ == rhs.lhs() && rhs_ == rhs.rhs();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const ConditionalAssertion & rhsCondAssertion = dynamic_cast<const ConditionalAssertion&>(rhs);
      return *this == rhsCondAssertion;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const ConditionalAssertion& rhs) const {
    return !(*this == rhs);
  }

 private:
  Fact lhs_;
  std::vector<BaseFact> rhs_;
};

// Assertions can have two forms:
//  - Facts
//  - Conditional assertions
class Assertion : public AstNode {
 public:
  // AssertionVariantType gives the different forms of Facts. Client code
  // should use this type to traverse these forms. This type may be changed in
  // the future.
  using AssertionVariantType = std::variant<Fact, ConditionalAssertion>;
  explicit Assertion(AssertionVariantType value) : value_(std::move(value)) {}
  virtual ~Assertion() = default;
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

  bool operator==(const Assertion& rhs) const {
    return value_ == rhs.GetValue();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Assertion & rhsAssertion = dynamic_cast<const Assertion&>(rhs);
      return *this == rhsAssertion;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Assertion& rhs) const { return !(*this == rhs); }

 private:
  AssertionVariantType value_;
};

// SaysAssertion prepends an assertion with "<principal> says"
class SaysAssertion : public AstNode {
 public:
  explicit SaysAssertion(Principal principal, std::vector<Assertion> assertions)
      : principal_(std::move(principal)), assertions_(std::move(assertions)) {}
  virtual ~SaysAssertion() = default;
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

  bool operator==(const SaysAssertion& rhs) const {
    return principal_ == rhs.principal() && assertions_ == rhs.assertions();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    if (const SaysAssertion* rhs_assertion = dynamic_cast<const SaysAssertion*>(&rhs)) {
      return *this == *rhs_assertion;
    }
    return false;
  }

  bool operator!=(const SaysAssertion& rhs) const { return !(*this == rhs); }

 private:
  Principal principal_;
  std::vector<Assertion> assertions_;
};

// A query which can be used to test whether "<principal> says <fact>" is true
// given the rules and base facts.
class Query : public AstNode {
 public:
  explicit Query(std::string name, Principal principal, Fact fact)
      : name_(std::move(name)),
        principal_(std::move(principal)),
        fact_(std::move(fact)) {}
  virtual ~Query() = default;
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


  bool operator==(const Query& rhs) const {
    return name_ == rhs.name() && principal_ == rhs.principal() &&
           fact_ == rhs.fact();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Query & rhsQuery = dynamic_cast<const Query&>(rhs);
      return *this == rhsQuery;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Query& rhs) const { return !(*this == rhs); }

 private:
  std::string name_;
  Principal principal_;
  Fact fact_;
};

// This is a top-level program.
class Program : public AstNode {
 public:
  explicit Program(
      std::vector<datalog::RelationDeclaration> relation_declarations,
      std::vector<SaysAssertion> says_assertions, std::vector<Query> queries)
      : relation_declarations_(std::move(relation_declarations)),
        says_assertions_(std::move(says_assertions)),
        queries_(std::move(queries)) {}
  virtual ~Program() = default;

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

  // for debugging/testing only
  bool operator==(const Program& rhs) const {
    return relation_declarations_ == rhs.relation_declarations() &&
           says_assertions_ == rhs.says_assertions() &&
           queries_ == rhs.queries();
  }
  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Program & rhsProgram = dynamic_cast<const Program&>(rhs);
      return *this == rhsProgram;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  bool operator!=(const Program& rhs) const { return !(*this == rhs); }

 private:
  std::vector<datalog::RelationDeclaration> relation_declarations_;
  std::vector<SaysAssertion> says_assertions_;
  std::vector<Query> queries_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
