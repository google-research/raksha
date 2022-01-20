/*
 * Copyright 2022 The Raksha Authors
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
#include <utility>  //included for std::pair

#include "absl/strings/str_format.h"
#include "src/common/logging/logging.h"
#include "src/ir/auth_logic/ast.h"
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

  // This function makes a copy of a predicate that prepends the name with
  // another string and prepends the list of arguments with additional
  // arguments. This is used in a few places in the translation, for
  // example, to translate "X says blah(args)" into "says_blah(X, args)".
  Predicate PushOntoPred(std::string modifier,
                         std::vector<std::string> new_args,
                         Predicate predicate) {
    std::string new_name = modifier + predicate.name();
    new_args.insert(new_args.end(),
                    std::make_move_iterator(predicate.args().begin()),
                    std::make_move_iterator(predicate.args().end()));
    Sign sign_copy = predicate.sign();
    return Predicate(new_name, new_args, sign_copy);
  }

  Predicate PushPrin(std::string modifier, Principal principal,
                     Predicate predicate) {
    return PushOntoPred(std::move(modifier), {principal.name()}, predicate);
  }

  Predicate AttributeToDLIR(Attribute attribute) {
    // If attribute is `X pred(args...)` the following predicate is
    // `pred(X, args...)`
    return PushPrin(std::string(""), attribute.principal(),
                    attribute.predicate());
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
  DLIRAssertion SpokenAttributeToDLIR(Principal speaker, Attribute attribute) {
    Predicate main_predicate = AttributeToDLIR(attribute);

    // Attributes interact with "canActAs" because if "Y canActAs X"
    // then Y also picks up X's attributes. We need to generate
    // an additional rule to implement this behavior. If the attribute
    // under translation is `X PredX`, the additional rule is:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    // (Where Y is a fresh variable)
    Principal prin_y = Principal(FreshVar());

    // This is `speaker says Y PredX`
    Predicate generated_lhs = PushPrin("says_", prin_y, main_predicate);

    Predicate y_can_act_as_x =
        Predicate("canActAs", {prin_y.name(), attribute.principal().name()},
                  Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrin("says_", speaker, y_can_act_as_x);

    // This is `speaker says X PredX`
    Predicate speaker_says_x_pred = PushPrin("says_", speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                        std::move(speaker_says_x_pred)}));
  }

  Predicate CanActAsToDLIR(CanActAs can_act_as) {
    return Predicate("canActAs",
                     {can_act_as.left_principal().name(),
                      can_act_as.right_principal().name()},
                     kPositive);
  }

  // In the same way that attributes are passed around with "CanActAs", so
  // are other "CanActAs" facts. To implement this, the translation of
  // CanActAs also results in both a predicate and an extra rule that passes
  // these facts around. This function is for generating the extra rule and
  // it works similarly to "SpokenAttributeToDLIR".
  DLIRAssertion SpokenCanActAsToDLIR(Principal speaker, CanActAs can_act_as) {
    Predicate main_predicate = CanActAsToDLIR(can_act_as);

    // "canActAs" facts are passed to principals via other canActAs facts in
    // essentially the same way as attributes. This function adds extra
    // rules to pass these around. If the `canActAs` under translation
    // is `X canActAs Z`, then the rule we need to generate is:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X canActAsZ`
    // (Where Y is a fresh variable)
    Principal prin_y = Principal(FreshVar());

    // This is `speaker says Y PredX`
    Predicate generated_lhs = PushPrin("says_", prin_y, main_predicate);

    Predicate y_can_act_as_x = Predicate(
        "canActAs", {prin_y.name(), can_act_as.left_principal().name()},
        Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrin("says_", speaker, y_can_act_as_x);

    // This is `speaker says X canActAs Z`
    Predicate speaker_says_x_can_act_as_z =
        PushPrin("says_", speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X canActAs Z`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                        std::move(speaker_says_x_can_act_as_z)}));
  }

  // The second return value represents 0 or 1 newly generated rules, so an
  // option might seem more intuitive. However, the interface that consumes
  // this needs to construct a vector anyway, so a vector is used in the
  // return type.
  std::pair<Predicate, std::vector<DLIRAssertion>> BaseFactToDLIR(
      Principal speaker, BaseFact base_fact) {
    return std::visit(
        raksha::utils::overloaded{
            // To make the typechecker happy the vectors need to be explicitly
            // declared.
            [](Predicate predicate) {
              std::vector<DLIRAssertion> pred_list = {};
              return std::make_pair(predicate, pred_list);
            },
            [this, &speaker](Attribute attribute) {
              std::vector<DLIRAssertion> pred_list = {
                  SpokenAttributeToDLIR(speaker, attribute)};
              return std::make_pair(AttributeToDLIR(attribute), pred_list);
            },
            [this, &speaker](CanActAs canActAs) {
              std::vector<DLIRAssertion> pred_list = {
                  SpokenCanActAsToDLIR(speaker, canActAs)};
              return std::make_pair(CanActAsToDLIR(canActAs), pred_list);
            }},
        base_fact.GetValue());
  }

  // This can result in 0 or more new rules because the translation of
  // nested canSayFacts might result in more than 1 rule.
  std::pair<Predicate, std::vector<DLIRAssertion>> FactToDLIR(
      Principal speaker, const Fact& fact) {
    return std::visit(
        raksha::utils::overloaded{
            [this, &speaker](BaseFact base_fact) {
              return BaseFactToDLIR(speaker, base_fact);
            },
            // To make the typechecker happy, this needs to be a const reference
            // to a unique ptr... which is surprising to me.
            [this, &speaker](const std::unique_ptr<CanSay>& can_say) {
              auto [inner_fact_dlir, gen_rules] =
                  FactToDLIR(speaker, *can_say->fact());

              Principal fresh_principal(FreshVar());

              // The following code generates the extra rule that does
              // delegation. This rule is:
              // ```
              // speaker says inner_fact_dlir:-
              //      fresh_principal says inner_fact_dlir,
              //      speaker says fresh_principal canSay inner_fact_dlir
              // ```

              // This is p says inner_fact_dlir
              auto lhs = PushPrin("says_", speaker, inner_fact_dlir);
              auto fresh_prin_says_inner =
                  PushPrin("says_", fresh_principal, inner_fact_dlir);
              auto speaker_says_fresh_cansay_inner = PushPrin(
                  "says_", speaker,
                  PushPrin("canSay_", fresh_principal, inner_fact_dlir));
              auto rhs = {fresh_prin_says_inner,
                          speaker_says_fresh_cansay_inner};
              DLIRAssertion generated_rule(DLIRCondAssertion(lhs, rhs));
              gen_rules.push_back(generated_rule);

              // Note that prin_cansay_pred does not begin with "speaker says"
              // because only the top-level fact should. This could
              // be a recursive call, so it might not be processing
              // the top-level fact. The top-level fact gets wrapped
              // in a "says" during the call to translate the assertion
              // in which this appears.
              auto prin_cansay_pred =
                  PushPrin("canSay_", can_say->principal(), inner_fact_dlir);
              return std::make_pair(prin_cansay_pred, gen_rules);
            }},
        fact.GetValue());
  }

  std::vector<DLIRAssertion> SingleSaysAssertionToDLIR(
      Principal speaker, const Assertion& assertion) {
    return std::visit(
        raksha::utils::overloaded{
            // I'm not sure why this signature needs to be a const reference to
            // make the typechecker happy when the visitor for BaseFactToDLIR
            // did
            // not need this.
            [this, &speaker](const Fact& fact) {
              auto [fact_predicate, generated_rules] =
                  FactToDLIR(speaker, fact);
              DLIRAssertion main_assertion =
                  DLIRAssertion(PushPrin("says_", speaker, fact_predicate));
              generated_rules.push_back(main_assertion);
              return generated_rules;
            },
            [this,
             &speaker](const ConditionalAssertion& conditional_assertion) {
              std::vector<Predicate> dlir_rhs = {};
              for (auto ast_rhs : conditional_assertion.rhs()) {
                // extra rule are only generated for facts on the LHS,
                // so the rules that would be generated from this RHS fact are
                // not used.
                auto [dlir_translation, not_used] =
                    BaseFactToDLIR(speaker, ast_rhs);
                dlir_rhs.push_back(
                    PushPrin("says_", speaker, dlir_translation));
              }
              auto [dlir_lhs, gen_rules] =
                  FactToDLIR(speaker, conditional_assertion.lhs());
              auto dlir_lhs_prime = PushPrin("says_", speaker, dlir_lhs);
              DLIRAssertion dlir_assertion(
                  DLIRCondAssertion(dlir_lhs_prime, dlir_rhs));
              gen_rules.push_back(dlir_assertion);
              return gen_rules;
            }},
        assertion.GetValue());
  }

  std::vector<DLIRAssertion> SaysAssertionToDLIR(
      const SaysAssertion& says_assertion) {
    std::vector<DLIRAssertion> ret = {};
    for (const Assertion& assertion : says_assertion.assertions()) {
      std::vector<DLIRAssertion> single_translation =
          SingleSaysAssertionToDLIR(says_assertion.principal(), assertion);
      ret.insert(ret.end(), std::make_move_iterator(single_translation.begin()),
                 std::make_move_iterator(single_translation.end()));
    }
    return ret;
  }

  std::vector<DLIRAssertion> SaysAssertionsToDLIR(
      const std::vector<SaysAssertion>& says_assertions) {
    std::vector<DLIRAssertion> ret = {};
    for (const SaysAssertion& says_assertion : says_assertions) {
      auto single_translation = SaysAssertionToDLIR(says_assertion);
      ret.insert(ret.end(), std::make_move_iterator(single_translation.begin()),
                 std::make_move_iterator(single_translation.end()));
    }
    return ret;
  }

  // Queries are like predicates with arity 0. Souffle does not have predicates
  // with arity 0, so we represent them as having one argument which is a
  // constant.
  static inline Predicate kDummyPredicate =
      Predicate("grounded_dummy", {"dummy_var"}, kPositive);

  DLIRAssertion QueryToDLIR(const Query& query) {
    // The assertions that are normally generated during the translation
    // from facts to dlir facts are not used for queries.
    auto [main_pred, not_used] = FactToDLIR(query.principal(), query.fact());
    main_pred = PushPrin("says_", query.principal(), main_pred);
    Predicate lhs(query.name(), {"dummy_var"}, kPositive);
    return DLIRAssertion(DLIRCondAssertion(lhs, {main_pred, kDummyPredicate}));
  }

  std::vector<DLIRAssertion> QueriesToDLIR(const std::vector<Query>& queries) {
    std::vector<DLIRAssertion> ret = {};
    for (const Query& query : queries) {
      ret.push_back(QueryToDLIR(query));
    }
    return ret;
  }

  DLIRProgram ProgToDLIR(const Program& program) {
    auto dlir_assertions = SaysAssertionsToDLIR(program.says_assertions());
    auto dlir_queries = QueriesToDLIR(program.queries());
    // We need to add a fact that says the dummy variable used in queries is
    // grounded.
    DLIRAssertion dummy_assertion(kDummyPredicate);
    dlir_assertions.insert(dlir_assertions.end(),
                           std::make_move_iterator(dlir_queries.begin()),
                           std::make_move_iterator(dlir_queries.end()));
    dlir_assertions.push_back(dummy_assertion);

    std::vector<std::string> outputs = {};
    for (const Query& query : program.queries()) {
      outputs.push_back(query.name());
    }
    return DLIRProgram(dlir_assertions, outputs);
  }

  uint64_t fresh_var_count_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
