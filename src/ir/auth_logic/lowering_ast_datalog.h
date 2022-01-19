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

#include <utility>  //included for std::pair

#include "absl/strings/str_format.h"
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
class LoweringToDatalogPass {
 public:
  static DLIRProgram Lower(Program auth_logic_program) {
    return LoweringToDatalogPass().ProgToDLIR(auth_logic_program);
  }

 private:
  LoweringToDatalogPass() : fresh_var_count_(0) {}

  int fresh_var_count_;

  // This returns a fresh variable name. Fresh variables are introduced in a
  // few places during the translation.
  std::string FreshVar() { return "x__" + std::to_string(++fresh_var_count_); }

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
    return PushOntoPred(modifier, {principal.name()}, predicate);
  }

  Predicate AttributeToDLIR(Attribute attribute) {
    // If attribute is `X pred(args...)` the following predicate is
    // `pred(X, args...)`
    return PushPrin(std::string(""), attribute.principal(),
                    attribute.predicate());
  }

  // This is the translation for the attribute case of a base fact.
  // This returns two outputs as a pair:
  // - A predicate, this is the main result of the translation
  // - An additional rule that allows attributes to be passed with canActAs
  // This takes both a speaker and the actual attribute as argument because
  // the speaker is needed to generate the additional rule. Note all base
  // facts have a speaker either explicitly or implicitly:
  // - If it appears on the LHS of an assertion, it explicitly has a speaker
  // - If it appears on the RHS of an assertion, it behaves semantically
  // like has the same speaker as the head of the assertion.
  DLIRAssertion SpokenAttributeToDLIR(Principal speaker,
                                      Attribute attribute) {
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
    Predicate generated_lhs =
        PushPrin(std::string("says_"), prin_y, main_predicate);

    Predicate y_can_act_as_x = Predicate(
        std::string("canActAs"), {prin_y.name(), attribute.principal().name()},
        Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrin(std::string("says_"), speaker, y_can_act_as_x);

    // This is `speaker says X PredX`
    Predicate speaker_says_x_pred =
        PushPrin(std::string("says_"), speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                        std::move(speaker_says_x_pred)}));
  }

  Predicate CanActAsToDLIR(CanActAs can_act_as) {
    return Predicate("canActAs", {can_act_as.left_principal().name(),
        can_act_as.right_principal().name()}, kPositive);
  }

  DLIRAssertion SpokenCanActAsToDLIR(Principal speaker,
                                  CanActAs can_act_as) {
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
    Predicate generated_lhs =
        PushPrin(std::string("says_"), prin_y, main_predicate);

    Predicate y_can_act_as_x = Predicate(std::string("canActAs"),
        {prin_y.name(), can_act_as.left_principal().name()},
        Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrin(std::string("says_"), speaker, y_can_act_as_x);

    // This is `speaker says X canActAs Z`
    Predicate speaker_says_x_can_act_as_z =
        PushPrin(std::string("says_"), speaker, main_predicate);

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
              std::vector<DLIRAssertion> pred_list = 
                {SpokenAttributeToDLIR(speaker, attribute)};
              return std::make_pair(AttributeToDLIR(attribute), pred_list);
            },
            [this, &speaker](CanActAs canActAs) {
              std::vector<DLIRAssertion> pred_list =
                {SpokenCanActAsToDLIR(speaker, canActAs)};
              return std::make_pair(CanActAsToDLIR(canActAs), pred_list);
            }},
        base_fact.GetValue());
  }

  // This can result in 0 or more new rules because the translation of
  // nested canSayFacts might result in more than 1 rule.
  std::pair<Predicate, std::vector<DLIRAssertion>> FactToDLIR(
      Principal speaker, const Fact& fact) {
      return std::visit(
        raksha::utils::overloaded {
          [this, &speaker](BaseFact base_fact) {
            return BaseFactToDLIR(speaker, base_fact);
          },
          // To make the typechecker happy, this needs to be a const reference
          // to a unique ptr... which is surprising to me.
          [this, &speaker](const std::unique_ptr<CanSay>& can_say) {
            auto[inner_fact_dlir, gen_rules] = FactToDLIR(speaker, *can_say->fact());

            auto fresh_principal = Principal(FreshVar());

            // The following code generates the extra rule that does 
            // delegation. This rule is:
            // ```
            // speaker says inner_fact_dlir:-
            //      fresh_principal says inner_fact_dlir,
            //      speaker says fresh_principal canSay inner_fact_dlir 
            // ```
          
            // This is p says inner_fact_dlir
            auto lhs = PushPrin(std::string("says_"), speaker, inner_fact_dlir);
            auto fresh_prin_says_inner = PushPrin(std::string("says_"),
                fresh_principal, inner_fact_dlir);
            auto speaker_says_fresh_cansay_inner = PushPrin(std::string("says_"),
                speaker, PushPrin(std::string("canSay_"),
                  fresh_principal, inner_fact_dlir));
            auto rhs = {fresh_prin_says_inner, speaker_says_fresh_cansay_inner};
            auto generated_rule = DLIRAssertion(DLIRCondAssertion(lhs, rhs));
            gen_rules.push_back(generated_rule);

            // Note that prin_cansay_pred does not begin with "speaker says"
            // because only the top-level fact should. This could
            // be a recursive call, so it might not be processing
            // the top-level fact. The top-level fact gets wrapped
            // in a "says" during the call to translate the assertion
            // in which this appears.
            auto prin_cansay_pred = PushPrin(std::string("canSay_"),
                can_say->principal(),
                inner_fact_dlir);
            return std::make_pair(prin_cansay_pred, gen_rules);
          }
        }, fact.GetValue());
  }

  std::vector<DLIRAssertion>
  SingleSaysAssertionToDLIR(Principal speaker,
      const Assertion& assertion) {
    return std::visit(
        raksha::utils::overloaded{
          // I'm not sure why this signature needs to be a const reference to
          // make the typechecker happy when the visitor for BaseFactToDLIR did
          // not need this.
          [this, &speaker](const Fact& fact) {
            auto[fact_predicate, generated_rules] = FactToDLIR(
                speaker, fact);
            DLIRAssertion main_assertion = DLIRAssertion(
                  PushPrin(std::string("says_"), speaker, fact_predicate));
            generated_rules.push_back(main_assertion);
            return generated_rules;
          },
          [this, &speaker](const ConditionalAssertion& conditional_assertion) {
          std::vector<Predicate> dlir_rhs = {};
            for (auto ast_rhs : conditional_assertion.rhs()) {
              // extra rule are only generated for facts on the LHS,
              // so the rules that would be generated from this RHS fact are 
              // not used.
              auto[dlir_translation, not_used] = BaseFactToDLIR(speaker, ast_rhs);
              dlir_rhs.push_back(PushPrin(std::string("says_"),
                    speaker, dlir_translation));
            }
            auto[dlir_lhs, gen_rules] = FactToDLIR(speaker,
                conditional_assertion.lhs());
            auto dlir_lhs_prime = PushPrin(std::string("says_"), speaker, dlir_lhs);
            auto dlir_assertion = DLIRAssertion(
                DLIRCondAssertion(dlir_lhs_prime, dlir_rhs));
            gen_rules.push_back(dlir_assertion);
            return gen_rules;
          }
        },
        assertion.GetValue());
  }

  std::vector<DLIRAssertion>
  SaysAssertionToDLIR(const SaysAssertion& says_assertion) {
    std::vector<DLIRAssertion> ret = {};
    for(const Assertion& assertion : says_assertion.assertions()) {
      std::vector<DLIRAssertion> single_translation =
        SingleSaysAssertionToDLIR(says_assertion.principal(), assertion);
      ret.insert(ret.end(),
          std::make_move_iterator(single_translation.begin()),
          std::make_move_iterator(single_translation.end()));
    }
    return ret;
  }

  std::vector<DLIRAssertion>
  SaysAssertionsToDLIR(const std::vector<SaysAssertion>& says_assertions) {
    std::vector<DLIRAssertion> ret = {};
    for(const SaysAssertion& says_assertion: says_assertions) {
      auto single_translation = SaysAssertionToDLIR(says_assertion);
      ret.insert(ret.end(),
          std::make_move_iterator(single_translation.begin()),
          std::make_move_iterator(single_translation.end()));
    }
    return ret;
  }

  // Queries are like predicates with arity 0. Souffle does not have predicates 
  // with arity 0, so we represent them as having one argument which is a 
  // constant. 
  #define DUMMY_PREDICATE Predicate("grounded_dummy", {"dummy_var"}, kPositive)

  DLIRAssertion
  QueryToDLIR(const Query& query) {
    // The assertions that are normally generated during the translation
    // from facts to dlir facts are not used for queries.
    auto[main_pred, not_used] = FactToDLIR(query.principal(), query.fact());
    main_pred = PushPrin(std::string("says_"), query.principal(), main_pred);
    auto lhs = Predicate(query.name(), {"dummy_var"}, kPositive);
    return DLIRAssertion(DLIRCondAssertion(
          lhs, {main_pred, DUMMY_PREDICATE}));
  }

  std::vector<DLIRAssertion>
  QueriesToDLIR(const std::vector<Query>& queries) {
    std::vector<DLIRAssertion> ret = {};
    for(const Query& query : queries) {
      ret.push_back(QueryToDLIR(query));
    }
    return ret;
  }

  DLIRProgram ProgToDLIR(const Program& program) {
    auto dlir_assertions = SaysAssertionsToDLIR(program.says_assertions());
    auto dlir_queries = QueriesToDLIR(program.queries());
    // We need to add a fact that says the dummy variable used in queries is 
    // grounded.
    auto dummy_assertion = DLIRAssertion(DUMMY_PREDICATE);
    dlir_assertions.insert(dlir_assertions.end(),
        std::make_move_iterator(dlir_queries.begin()),
        std::make_move_iterator(dlir_queries.end()));
    dlir_assertions.push_back(dummy_assertion);

    std::vector<std::string> outputs = {};
    for(const Query& query: program.queries()) {
      outputs.push_back(query.name());
    }
    return DLIRProgram(dlir_assertions, outputs);
  }
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
