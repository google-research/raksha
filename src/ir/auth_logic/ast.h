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
  Principal(std::string name) : name_(std::move(name)) {}

 private:
  std::string name_;
};

// Used to represent whether a predicate is negated or not
enum Sign { kNegated, kPositive };

class Predicate {
 public:
  Predicate(std::string name, std::vector<std::string> args, Sign sign)
      : name_(std::move(name)),
        args_(std::move(args)),
        // TODO move surpresses an unused private field error about sign_
        // for now, get rid of this eventually
        sign_(std::move(sign)) {}

 private:
  std::string name_;
  std::vector<std::string> args_;
  Sign sign_;
};

// This represents an expression that applies either a predicate or a
// "canActAs" to a principal, and it is a sub-term of FlatFact
//    - pred_name(arg_1, ..., arg_n)           (a predicate)
//    - principal_1 canActAs principal_2       (a canActAs)
// To encapsulate which form this takes, it:
//    - uses factory methods to make either form
//    - uses a private std::variant member that takes either of these two forms
//    - uses std::visit to do operations on the actual state
class VerbPhrase {
 public:
  static std::unique_ptr<VerbPhrase> CreateVerbPhrase(Predicate pred) {
    return std::unique_ptr<VerbPhrase>(new VerbPhrase(std::move(pred)));
  }
  static std::unique_ptr<VerbPhrase> CreateVerbPhrase(Principal right_prin) {
    return std::unique_ptr<VerbPhrase>(new VerbPhrase(std::move(right_prin)));
  }

 private:
  // The principal variant of the internal form means this represents
  // "canActAs principal".
  std::variant<Predicate, Principal> internal_form_;
  VerbPhrase(std::variant<Predicate, Principal> internal_form)
      : internal_form_(std::move(internal_form)) {}
};

// This the AST Node for FlatFact which takes on one of two forms:
//   - <principal> <verb_phrase>  (* a principal applied to a verbphrase *)
//   - <predicate>                (* just a predicate *)
// A similar variant-based pattern is used for this as for VerbPhrase. Here an
// inner struct is used for the first form (principal applied to verb_phrase).
class FlatFact {
 public:
  static std::unique_ptr<FlatFact> CreateFlatFact(Principal prin,
                                                  VerbPhrase verb_phrase) {
    return std::unique_ptr<FlatFact>(
        new FlatFact(VerbPhraseFlatFact(prin, verb_phrase)));
  }
  static std::unique_ptr<FlatFact> CreateFlatFact(Predicate prin) {
    return std::unique_ptr<FlatFact>(new FlatFact(std::move(prin)));
  }

 private:
  struct VerbPhraseFlatFact {
    Principal prin;
    VerbPhrase verb_phrase;
    VerbPhraseFlatFact(Principal prin, VerbPhrase verb_phrase)
        : prin(std::move(prin)), verb_phrase(std::move(verb_phrase)) {}
  };
  std::variant<VerbPhraseFlatFact, Predicate> internal_form_;
  FlatFact(std::variant<VerbPhraseFlatFact, Predicate> internal_form)
      : internal_form_(std::move(internal_form)) {}
};

// This is the AST node for facts which have two forms:
//  - <flat_fact>
//  - <principal> canSay <flat_fact>
class Fact {
 public:
  static std::unique_ptr<Fact> CreateFact(FlatFact flat_fact) {
    return std::unique_ptr<Fact>(new Fact(std::move(flat_fact)));
  }
  static std::unique_ptr<Fact> CreateFact(Principal prin, Fact* fact) {
    return std::unique_ptr<Fact>(
        new Fact(CanSayFact(std::move(prin), std::move(fact))));
  }

 private:
  struct CanSayFact {
    Principal prin;
    Fact* fact;
    CanSayFact(Principal prin, Fact* fact)
        : prin(std::move(prin)), fact(fact) {}
  };
  std::variant<FlatFact, CanSayFact> internal_form_;
  Fact(std::variant<FlatFact, CanSayFact> internal_form)
      : internal_form_(std::move(internal_form)){};
};

// AST Node for Assertion (this is a sub-expression of SaysAssertion)
// which has two forms:
//  - <fact>                                      (unconditional)
//  - <fact> :- <flat_fact_1> ... <flat_fact_n>   (conditional)
class Assertion {
 public:
  static std::unique_ptr<Assertion> CreateAssertion(Fact fact) {
    return std::unique_ptr<Assertion>(new Assertion(std::move(fact)));
  }
  static std::unique_ptr<Assertion> CreateAssertion(Fact lhs,
                                                    std::vector<FlatFact> rhs) {
    return std::unique_ptr<Assertion>(
        new Assertion(CondAssertion(std::move(lhs), std::move(rhs))));
  }

 private:
  struct CondAssertion {
    Fact lhs;
    std::vector<FlatFact> rhs;
    CondAssertion(Fact lhs, std::vector<FlatFact> rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  };
  std::variant<Fact, CondAssertion> internal_form_;
  Assertion(std::variant<Fact, CondAssertion> internal_form)
      : internal_form_(std::move(internal_form)){};
};

// AST node for saysAssertion, which prepends an assertion with:
// "<principal> says"
class SaysAssertion {
 public:
  SaysAssertion(Principal prin, std::vector<Assertion> assertions)
      : prin_(std::move(prin)), assertions_(std::move(assertions)){};

 private:
  Principal prin_;
  std::vector<Assertion> assertions_;
};

// A query which can be used to test whether "<principal> says <fact>" is true
// given the rules and base facts.
class Query {
 public:
  Query(std::string name, Principal prin, Fact fact)
      : name_(std::move(name)),
        prin_(std::move(prin)),
        fact_(std::move(fact)) {}

 private:
  std::string name_;
  Principal prin_;
  Fact fact_;
};

class Program {
 public:
  Program(std::vector<SaysAssertion> says_assertions,
          std::vector<Query> queries)
      : says_assertions_(std::move(says_assertions)),
        queries_(std::move(queries)){};

 private:
  std::vector<SaysAssertion> says_assertions_;
  std::vector<Query> queries_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
