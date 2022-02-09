/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.
#ifndef SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_
#define SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_

#include <limits>
#include <memory>
#include <utility>  //included for std::pair

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "src/common/logging/logging.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/datalog_ir.h"
#include "src/ir/auth_logic/move_append.h"
#include "src/utils/overloaded.h"

namespace raksha::ir::auth_logic {

// This implements a compiler pass for converting authorization logic
// (represented by ast.h) into the datalog ir (in datalog_ir.h). The only
// public interface to this class is a single function "Lower" which takes
// as input an authorization logic program and returns a datalog program.
//
// (Internally, this is a stateful class because it uses a counter to
// generate fresh names for arguments. The public interface hides this
// implementation detail.)

//-----------------------------------------------------------------------------
// Description of Translation
//-----------------------------------------------------------------------------
// Asside from queries, this translation is quite similar to the one given in
// Section 7 of the SecPal paper "SecPAL: Design and Semantics of a
// Decentralized Authorization Language"
// https://www.pure.ed.ac.uk/ws/files/17662538/jcs_final.pdf. See that section
// for a formal and completely general description of this translation. This
// comment gives a more intuitive description using a few examples at the
// expense of losing some generality.
//
// As a minor wrinkle, this language also supports grouping assertions together
// as in:
//      Prin says { assertion_1, ..., assertion_n }
// and these are simply "unrolled" to
//      Prin says assertion_1.
//      ...
//      Prin says assertion_n.
//
//
// ## Translation of assertions
//
// First, predicates with modifiers like "says", "canSay", and "actAs" are
// converted into normal predicates by appending these modifiers to the
// predicate name and moving the principals referenced by these predicates
// into the arguments of the predicates. So for example
//         A says B canSay foo(x, y)
// becomes
//         says_canSay_foo(A, B, x, y)
// The rest of this comment ignores this part of the translation for the sake
// of readability.
//
// Next we need to add a principal for the conditions in assertions. To prove
// an assertion like
//         "A says fact(x) :- fact1(x), fact2(x).
// we need to show that *A believes* both fact1(x) and fact2(x). So we prepend
// predicates on the RHS in "A says", producing the new assertion:
//         A says fact(x) :- A says fact1(x), A says fact2(x)
// (where says is actually prepended to the names of predicates and A becomes
// an extra argument as above).
//
//  Next we need to translate delegations which should pass beliefs from one
//  principal to another. For example, if we have
//      A says B canSay fact(x) :- fact1(x)
//  we do the usual translation resulting in
//      (1) A says B canSay fact(x) :- A says fact1(x)
//  but we also add the additional rule
//      (2) A says fact(x) :- x says fact(x), A says x canSay fact(x)
//  where x is a fresh variable.
//  The result is that rule (1) gives the condition for delegation
//  and rule (2) passes beliefs from B to A when the condition from (1) is met.
//
// Finally we need to translate canActAs which should pass properties from
// one principal to another:
// Assertions of either the form
//      A says B canActAs C :- ...
// or
//      A says B <anyPredicate(...)> :-
// are claims by A that B has a property (where canActAs is just a special
// case). Syntactically these are both treated as "A says B <verbphrase>" with
// different instances of <verbphrase>. Whenever some other principal can act
// as B, we want to pass these properties from B to that other principal.
// (For the special case where this property is B canActAs C, this essentially
// makes canActAs transitive). In either case, we add an additional rule
//    A says x verbphrase <- A says x canActAs B, A says B verbphrase
// which will add "verbphrase" as a property of D whenever we can prove
// the assertion "A says D canActAsB".
//
// ## Translation of Queries
// Neither Souffle nor SecPal support queries, but this language does.
// Queries check of a single unconditional assertion is true. A query of
// the form:
// ```Datalog
//      Q_NAME = query ASSERTION(<ARGS>) ?
// ```
//
// is translated into an assertion with just one argument, and this
// assertion is made an output, so we can simply check the CSV that
// souffle emits as in:
// ```Datalog
//      Q_NAME("dummy_var") :- grounded("dummy_var"), ASSERTION(<ARGS>).
//      grounded("dummy_var").
//      .output Q_NAME
// ```

class LoweringToDatalogPass {
 public:
  static DLIRProgram Lower(const Program& auth_logic_program) {
    return LoweringToDatalogPass().ProgToDLIR(auth_logic_program);
  }

 private:
  LoweringToDatalogPass() : fresh_var_count_(0) {}

  // This returns a fresh variable name. Fresh variables are introduced in a
  // few places during the translation.
  std::string FreshVar() {
    CHECK(fresh_var_count_ < std::numeric_limits<uint64_t>::max());
    return "x__" + std::to_string(++fresh_var_count_);
  }

  // Translating an attribute results in two objects:
  // - A predicate, this is the main result of the translation
  // - An additional rule that allows attributes to be passed with canActAs
  // This function is for generating the additional rule.
  // This takes both a speaker and the actual attribute as argument because
  // the speaker is needed to generate the additional rule. Note that all base
  // facts have a speaker either explicitly or implicitly:
  // - If it appears on the LHS of an assertion, it explicitly has a speaker
  // - If it appears on the RHS of an assertion, it behaves semantically
  // like it has the same speaker as the head of the assertion.
  DLIRAssertion SpokenAttributeToDLIR(const Principal& speaker,
                                      const Attribute& attribute);

  Predicate CanActAsToDLIR(const CanActAs& can_act_as);

  // In the same way that attributes are passed around with "CanActAs", so
  // are other "CanActAs" facts. To implement this, the translation of
  // CanActAs also results in both a predicate and an extra rule that passes
  // these facts around. This function is for generating the extra rule and
  // it works similarly to "SpokenAttributeToDLIR".
  DLIRAssertion SpokenCanActAsToDLIR(const Principal& speaker,
                                     const CanActAs& can_act_as);

  std::pair<Predicate, std::vector<DLIRAssertion>> BaseFactToDLIRInner(
      const Principal& speaker, const Predicate& predicate);

  std::pair<Predicate, std::vector<DLIRAssertion>> BaseFactToDLIRInner(
      const Principal& speaker, const Attribute& attribute);

  std::pair<Predicate, std::vector<DLIRAssertion>> BaseFactToDLIRInner(
      const Principal& speaker, const CanActAs& canActAs);

  // The second return value represents 0 or 1 newly generated rules, so an
  // option might seem more intuitive. However, the interface that consumes
  // this needs to construct a vector anyway, so a vector is used in the
  // return type.
  std::pair<Predicate, std::vector<DLIRAssertion>> BaseFactToDLIR(
      const Principal& speaker, const BaseFact& base_fact);

  // This can result in 0 or more new rules because the translation of
  // nested canSayFacts might result in more than 1 rule.
  std::pair<Predicate, std::vector<DLIRAssertion>> FactToDLIR(
      const Principal& speaker, const Fact& fact);

  std::vector<DLIRAssertion> GenerateDLIRAssertions(const Principal& speaker,
                                                    const Fact& fact);

  std::vector<DLIRAssertion> GenerateDLIRAssertions(
      const Principal& speaker,
      const ConditionalAssertion& conditional_assertion);

  std::vector<DLIRAssertion> SingleSaysAssertionToDLIR(
      const Principal& speaker, const Assertion& assertion);

  std::vector<DLIRAssertion> SaysAssertionToDLIR(
      const SaysAssertion& says_assertion);

  std::vector<DLIRAssertion> SaysAssertionsToDLIR(
      const std::vector<SaysAssertion>& says_assertions);

  // Queries are like predicates with arity 0. Souffle does not have predicates
  // with arity 0, so we represent them as having one argument which is a
  // constant.
  static inline Predicate kDummyPredicate =
      Predicate("grounded_dummy", {"dummy_var"}, kPositive);

  std::vector<DLIRAssertion> QueriesToDLIR(const std::vector<Query>& queries);

  DLIRProgram ProgToDLIR(const Program& program);

  uint64_t fresh_var_count_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
