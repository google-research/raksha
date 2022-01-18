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

#include <string>
#include <variant>
#include <vector>

namespace raksha::ir::auth_logic {

class Principal {
 public:
  explicit Principal(std::string name) : name_(std::move(name)) {}
  const std::string& name() const { return name_; }

 private:
  std::string name_;
};

// Used to represent whether a predicate is negated or not
enum Sign { kNegated, kPositive };

// Predicate corresponds to a predicate of the form
// <pred_name>(arg_1, ..., arg_n), and it may or may not be negated
class Predicate {
 public:
  Predicate(std::string name, std::vector<std::string> args, Sign sign)
      : name_(std::move(name)),
        args_(std::move(args)),
        // TODO move surpresses an unused private field error about sign_
        // for now, get rid of this eventually
        sign_(std::move(sign)) {}

  const std::string& name() const { return name_; }
  const std::vector<std::string>& args() const { return args_; }
  Sign sign() const { return sign_; }

 private:
  std::string name_;
  std::vector<std::string> args_;
  Sign sign_;
};

// Attribute corresponds to an attribute of a principal with the form
// <Principal> <pred_name>(arg_1, ..., arg_n)
class Attribute {
 public:
  explicit Attribute(Principal principal, Predicate predicate)
      : principal_(principal), predicate_(predicate) {}
  const Principal& principal() const { return principal_; }
  const Predicate& predicate() const { return predicate_; }

 private:
  Principal principal_;
  Predicate predicate_;
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
using BaseFactVariantType = std::variant<Predicate, Attribute, CanActAs>;
class BaseFact {
 public:
  explicit BaseFact(Predicate predicate) : value_(std::move(predicate)){};
  explicit BaseFact(Attribute attribute) : value_(std::move(attribute)){};
  explicit BaseFact(CanActAs canActAs) : value_(std::move(canActAs)){};
  const BaseFactVariantType& GetValue() const { return value_; }

 private:
  BaseFactVariantType value_;
};

// CanSay and Fact are forward-declared because they are mutually recursive.
class CanSay;
class Fact;

// CanSay corresponds to an expression of the form <principal> canSay Fact
class CanSay {
 public:
  explicit CanSay(Principal principal, std::unique_ptr<Fact>& fact)
      : principal_(principal), fact_(std::move(fact)) {}
  const Principal& principal() const { return principal_; }
  const Fact* fact() const { return fact_.get(); }

 private:
  Principal principal_;
  std::unique_ptr<Fact> fact_;
};

// Fact corresponds to either a base fact or a an expression of the form
// <principal> canSay <Fact>
using FactVariantType = std::variant<BaseFact, std::unique_ptr<CanSay>>;
class Fact {
 public:
  explicit Fact(BaseFact base_fact) : value_(std::move(base_fact)) {}
  explicit Fact(std::unique_ptr<CanSay> can_say) : value_(std::move(can_say)) {}
  const FactVariantType& GetValue() const { return value_; }

 private:
  FactVariantType value_;
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

 private:
  Fact lhs_;
  std::vector<BaseFact> rhs_;
};

// Assertions can have two forms:
//  - Facts
//  - Conditional assertions
using AssertionVariantType = std::variant<Fact, ConditionalAssertion>;
class Assertion {
 public:
  explicit Assertion(Fact fact) : value_(std::move(fact)) {}
  explicit Assertion(ConditionalAssertion conditional_assertion)
      : value_(std::move(conditional_assertion)) {}
  const AssertionVariantType& GetValue() const { return value_; }

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

 private:
  std::string name_;
  Principal principal_;
  Fact fact_;
};

// This is a top-level program.
class Program {
 public:
  explicit Program(std::vector<SaysAssertion> says_assertions,
                   std::vector<Query> queries)
      : says_assertions_(std::move(says_assertions)),
        queries_(std::move(queries)) {}
  const std::vector<SaysAssertion>& says_assertions() const {
    return says_assertions_;
  }
  const std::vector<Query>& queries() const { return queries_; }

 private:
  std::vector<SaysAssertion> says_assertions_;
  std::vector<Query> queries_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
