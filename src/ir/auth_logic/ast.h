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
#include <vector>
#include <variant>

namespace raksha::ir::auth_logic {

class AstPrincipal {
  public:
    AstPrincipal(std::string name) : name_(std::move(name)) {}
  private:
    std::string name_;
};

// Used to represent whether a predicate is negated or not
enum AstSign { negated, positive };

class AstPredicate {
  public:
    AstPredicate(
        std::string name,
        std::vector<std::string> args,
        AstSign sign)
      : name_(std::move(name)),
        args_(std::move(args)),
        sign_(std::move(sign)) {}
  private:
    std::string name_;
    std::vector<std::string> args_;
    AstSign sign_;
};

// This represents an expression that applies either a predicate or a 
// "canActAs" to a principal, and it is a sub-term of AstFlatFact
//    - pred_name(arg_1, ..., arg_n)           (a predicate)
//    - principal_1 canActAs principal_2       (a canActAs)
// To encapsulate which form this takes, it:
//    - uses factory methods to make either form
//    - uses a private std::variant member that takes either of these two forms
//    - uses std::visit to do operations on the actual state
class AstVerbPhrase {
  public:
    std::unique_ptr<AstVerbPhrase>
      CreateVerbPhrase(AstPredicate pred);
    std::unique_ptr<AstVerbPhrase>
      CreateVerbPhrase(AstPrincipal right_prin);
  private:
    // The principal variant of the internal form means this represents
    // "canActAs principal".
    using InternalFormType = std::variant<AstPredicate, AstPrincipal>;
    InternalFormType internal_form_;
    AstVerbPhrase(InternalFormType internal_form)
      : internal_form_(std::move(internal_form)) {}
};

// This the AST Node for FlatFact which takes on one of two forms:
//   - <principal> <verb_phrase>  (* a principal applied to a verbphrase *)
//   - <predicate>                (* just a predicate *)
// A similar variant-based pattern is used for this as for VerbPhrase. Here an 
// inner struct is used for the first form (principal applied to verb_phrase).
class AstFlatFact {
  public:
    std::unique_ptr<AstFlatFact>
      CreateFlatFact(AstPrincipal prin, AstVerbPhrase verb_phrase);
    std::unique_ptr<AstFlatFact>
      CreateFlatFact(AstPredicate prin);
  private:
    typedef struct VerbPhraseFlatFact {
      AstPrincipal prin;
      AstVerbPhrase verb_phrase;
      VerbPhraseFlatFact(AstPrincipal prin, AstVerbPhrase verb_phrase)
        : prin(std::move(prin)),
          verb_phrase(std::move(verb_phrase)) {}
    } VerbPhraseFlatFact;
    using InternalFormType = std::variant<VerbPhraseFlatFact, AstPredicate>;
    InternalFormType internal_form_;
    AstFlatFact(InternalFormType internal_form)
        : internal_form_(std::move(internal_form)) {}
};

// This is the AST node for facts which have two forms:
//  - <flat_fact>
//  - <principal> canSay <flat_fact>
class AstFact {
  public:
    std::unique_ptr<AstFact> CreateFact(AstFlatFact flat_fact);
    std::unique_ptr<AstFact> CreateFact(AstPrincipal prin, AstFact* fact);
  private:
    typedef struct CanSayFact {
      AstPrincipal prin;
      AstFact *fact;
      CanSayFact(AstPrincipal prin, AstFact* fact)
        : prin(std::move(prin)),
          fact(fact) {}
    } CanSayFact;
    using InternalFormType = std::variant<AstFlatFact, CanSayFact>;
    InternalFormType internal_form_;
    AstFact(InternalFormType internal_form)
      : internal_form_(std::move(internal_form)) {};
};

// AST Node for AstAssertion (this is a sub-expression of SaysAstAssertion) 
// which has two forms:
//  - <fact>                                      (unconditional)
//  - <fact> :- <flat_fact_1> ... <flat_fact_n>   (conditional)
class AstAssertion {
  public:
    std::unique_ptr<AstAssertion> CreateAssertion(AstFact fact);
    std::unique_ptr<AstAssertion> CreateAssertion(AstFact lhs,
        std::vector<AstFlatFact> rhs);
  private:
    typedef struct CondAssertion {
      AstFact lhs;
      std::vector<AstFlatFact> rhs;
      CondAssertion(AstFact lhs, std::vector<AstFlatFact> rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    } CondAssertion;
    using InternalFormType = std::variant<AstFact, CondAssertion>;
    InternalFormType internal_form_;
    AstAssertion(InternalFormType internal_form)
      : internal_form_(std::move(internal_form)) {};
};

// AST node for saysAssertion, which prepends an assertion with:
// "<principal> says"
class AstSaysAssertion {
  public:
    AstSaysAssertion(AstPrincipal prin, std::vector<AstAssertion> assertions)
      : prin_(std::move(prin)), assertions_(std::move(assertions)) {};
  private:
    AstPrincipal prin_;
    std::vector<AstAssertion> assertions_;
};

// A query which can be used to test whether "<principal> says <fact>" is true
// given the rules and base facts.
class AstQuery {
  public:
    AstQuery(std::string name, AstPrincipal prin, AstFact fact)
      : name_(std::move(name)),
        prin_(std::move(prin)),
        fact_(std::move(fact)) {}
  private:
    std::string name_;
    AstPrincipal prin_;
    AstFact fact_;
};

class AstProgram {
  public:
    AstProgram(std::vector<AstSaysAssertion> says_assertions,
        std::vector<AstQuery> queries)
      : says_assertions_(says_assertions),
        queries_(queries) {}; 
  private:
    std::vector<AstSaysAssertion> says_assertions_;
    std::vector<AstQuery> queries_;
};

} // namespace raksha::ir::auth_logic

#endif // SRC_IR_AUTH_LOGIC_AST_H_
