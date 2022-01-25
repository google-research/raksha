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

#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/lowering_ast_datalog.h"

namespace raksha::ir::auth_logic {

  DLIRAssertion 
  LoweringToDatalogPass::SpokenAttributeToDLIR(
        Principal speaker, Attribute attribute) {
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
    Predicate generated_lhs = PushPrincipal("says_", prin_y, main_predicate);

    Predicate y_can_act_as_x =
        Predicate("canActAs", {prin_y.name(), attribute.principal().name()},
                  Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrincipal("says_", speaker, y_can_act_as_x);

    // This is `speaker says X PredX`
    Predicate speaker_says_x_pred = PushPrincipal("says_", speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                        std::move(speaker_says_x_pred)}));
  }

  DLIRAssertion 
  LoweringToDatalogPass::SpokenCanActAsToDLIR(const Principal& speaker,
      const CanActAs& can_act_as) {
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
    Predicate generated_lhs = PushPrincipal("says_", prin_y, main_predicate);

    Predicate y_can_act_as_x = Predicate(
        "canActAs", {prin_y.name(), can_act_as.left_principal().name()},
        Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrincipal("says_", speaker, y_can_act_as_x);

    // This is `speaker says X canActAs Z`
    Predicate speaker_says_x_can_act_as_z =
        PushPrincipal("says_", speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X canActAs Z`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                        std::move(speaker_says_x_can_act_as_z)}));
  }
  
  std::pair<Predicate, std::vector<DLIRAssertion>>
  LoweringToDatalogPass::FactToDLIR(const Principal& speaker,
      const Fact& fact) {
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
              auto lhs = PushPrincipal("says_", speaker, inner_fact_dlir);
              auto fresh_prin_says_inner =
                  PushPrincipal("says_", fresh_principal, inner_fact_dlir);
              auto speaker_says_fresh_cansay_inner = PushPrincipal(
                  "says_", speaker,
                  PushPrincipal("canSay_", fresh_principal, inner_fact_dlir));
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
                  PushPrincipal("canSay_", can_say->principal(), inner_fact_dlir);
              return std::make_pair(prin_cansay_pred, gen_rules);
            }},
        fact.GetValue());
  }

  std::vector<DLIRAssertion>
  LoweringToDatalogPass::SingleSaysAssertionToDLIR(
      const Principal& speaker, const Assertion& assertion) {
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
                  DLIRAssertion(PushPrincipal("says_", speaker, fact_predicate));
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
                    PushPrincipal("says_", speaker, dlir_translation));
              }
              auto [dlir_lhs, gen_rules] =
                  FactToDLIR(speaker, conditional_assertion.lhs());
              auto dlir_lhs_prime = PushPrincipal("says_", speaker, dlir_lhs);
              DLIRAssertion dlir_assertion(
                  DLIRCondAssertion(dlir_lhs_prime, dlir_rhs));
              gen_rules.push_back(dlir_assertion);
              return gen_rules;
            }},
        assertion.GetValue());
  }

}  // namespace raksha::ir::auth_logic
