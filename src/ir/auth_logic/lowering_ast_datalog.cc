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

#include "src/ir/auth_logic/lowering_ast_datalog.h"

#include "src/ir/auth_logic/ast.h"
#include "src/utils/map_iter.h"
#include "src/utils/move_append.h"

namespace raksha::ir::auth_logic {

namespace {
// This function takes a `predicate` and creates a new predicate as follows:
//   - Prepend the `modifier` to the name of the given `predicate`.
//   - Prepend the given `args` to the original list of arguments of
//   `predicate`.
//
// This is used in a few places in the translation, for example, to translate
// "X says blah(args)" into "says_blah(X, args)".
Predicate PushOntoPredicate(absl::string_view modifier,
                            std::vector<std::string> new_args,
                            const Predicate& predicate) {
  std::string new_name = absl::StrCat(std::move(modifier), predicate.name());
  MoveAppend(new_args, std::vector<std::string>(predicate.args()));
  Sign sign_copy = predicate.sign();
  return Predicate(new_name, std::move(new_args), sign_copy);
}

// This function is an abbreviation for `PushPrincipal` where:
// - a modifier is added
// - just one new principal is added as an argument.
// This is a common case in this translation because it is used for
// `x says blah(args)` and `x canActAs y` and other constructions involving a
// principal name.
Predicate PushPrincipal(absl::string_view modifier, const Principal& principal,
                        const Predicate& predicate) {
  return PushOntoPredicate(modifier, {principal.name()}, predicate);
}

Predicate AttributeToDLIR(const Attribute& attribute) {
  // If attribute is `X pred(args...)` the following predicate is
  // `pred(X, args...)`
  return PushPrincipal("", attribute.principal(), attribute.predicate());
}

Predicate CanActAsToDLIR(const CanActAs& can_act_as) {
  return Predicate(
      "canActAs",
      {can_act_as.left_principal().name(), can_act_as.right_principal().name()},
      kPositive);
}
}  // namespace

DLIRAssertion LoweringToDatalogPass::SpokenAttributeToDLIR(
    const Principal& speaker, const Attribute& attribute) {
  Predicate main_predicate = AttributeToDLIR(attribute);

  // Attributes interact with "canActAs" because if "Y canActAs X"
  // then Y also picks up X's attributes. We need to generate
  // an additional rule to implement this behavior. If the attribute
  // under translation is `X PredX`, the additional rule is:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X PredX`
  // (Where Y is a fresh variable)
  Principal prin_y(FreshVar());

  // This is `speaker says Y PredX`
  Predicate generated_lhs = PushPrincipal("says_", prin_y, main_predicate);

  Predicate y_can_act_as_x("canActAs",
                           {prin_y.name(), attribute.principal().name()},
                           Sign::kPositive);

  Predicate speaker_says_y_can_act_as_x =
      PushPrincipal("says_", speaker, y_can_act_as_x);

  // This is `speaker says X PredX`
  Predicate speaker_says_x_pred =
      PushPrincipal("says_", speaker, main_predicate);

  // This is the full generated rule:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X PredX`
  return DLIRAssertion(
      DLIRCondAssertion(generated_lhs, {std::move(speaker_says_y_can_act_as_x),
                                        std::move(speaker_says_x_pred)}));
}

DLIRAssertion LoweringToDatalogPass::SpokenCanActAsToDLIR(
    const Principal& speaker, const CanActAs& can_act_as) {
  Predicate main_predicate = CanActAsToDLIR(can_act_as);

  // "canActAs" facts are passed to principals via other canActAs facts in
  // essentially the same way as attributes. This function adds extra
  // rules to pass these around. If the `canActAs` under translation
  // is `X canActAs Z`, then the rule we need to generate is:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X canActAsZ`
  // (Where Y is a fresh variable)
  Principal prin_y(FreshVar());

  // This is `speaker says Y PredX`
  Predicate generated_lhs = PushPrincipal("says_", prin_y, main_predicate);

  Predicate y_can_act_as_x("canActAs",
                           {prin_y.name(), can_act_as.left_principal().name()},
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
LoweringToDatalogPass::BaseFactToDLIRInner(const Principal& speaker,
                                           const Predicate& predicate) {
  return std::make_pair(predicate, std::vector<DLIRAssertion>({}));
}

std::pair<Predicate, std::vector<DLIRAssertion>>
LoweringToDatalogPass::BaseFactToDLIRInner(const Principal& speaker,
                                           const Attribute& attribute) {
  return std::make_pair(
      AttributeToDLIR(attribute),
      std::vector<DLIRAssertion>({SpokenAttributeToDLIR(speaker, attribute)}));
}

std::pair<Predicate, std::vector<DLIRAssertion>>
LoweringToDatalogPass::BaseFactToDLIRInner(const Principal& speaker,
                                           const CanActAs& canActAs) {
  return std::make_pair(
      CanActAsToDLIR(canActAs),
      std::vector<DLIRAssertion>({SpokenCanActAsToDLIR(speaker, canActAs)}));
}

std::pair<Predicate, std::vector<DLIRAssertion>>
LoweringToDatalogPass::BaseFactToDLIR(const Principal& speaker,
                                      const BaseFact& base_fact) {
  return std::visit(
      [this, &speaker](auto value) {
        return BaseFactToDLIRInner(speaker, value);
      },
      base_fact.GetValue());
}

std::pair<Predicate, std::vector<DLIRAssertion>>
LoweringToDatalogPass::FactToDLIR(const Principal& speaker, const Fact& fact) {
  return std::visit(
      raksha::utils::overloaded{
          [this, speaker](const BaseFact& base_fact) {
            return BaseFactToDLIR(speaker, base_fact);
          },
          [this, speaker](const std::unique_ptr<CanSay>& can_say) {
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
            auto rhs = {fresh_prin_says_inner, speaker_says_fresh_cansay_inner};
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

std::vector<DLIRAssertion> LoweringToDatalogPass::GenerateDLIRAssertions(
    const Principal& speaker, const Fact& fact) {
  auto [fact_predicate, generated_rules] = FactToDLIR(speaker, fact);
  DLIRAssertion main_assertion =
      DLIRAssertion(PushPrincipal("says_", speaker, fact_predicate));
  generated_rules.push_back(main_assertion);
  return generated_rules;
}

std::vector<DLIRAssertion> LoweringToDatalogPass::GenerateDLIRAssertions(
    const Principal& speaker,
    const ConditionalAssertion& conditional_assertion) {
  auto dlir_rhs = MapIter<BaseFact, Predicate>(
      conditional_assertion.rhs(), [this, speaker](const BaseFact& base_fact) {
        auto [dlir_translation, not_used] = BaseFactToDLIR(speaker, base_fact);
        return PushPrincipal("says_", speaker, dlir_translation);
      });
  auto [dlir_lhs, gen_rules] = FactToDLIR(speaker, conditional_assertion.lhs());
  auto dlir_lhs_prime = PushPrincipal("says_", speaker, dlir_lhs);
  DLIRAssertion dlir_assertion(DLIRCondAssertion(dlir_lhs_prime, dlir_rhs));
  gen_rules.push_back(dlir_assertion);
  return gen_rules;
}

std::vector<DLIRAssertion> LoweringToDatalogPass::SingleSaysAssertionToDLIR(
    const Principal& speaker, const Assertion& assertion) {
  return std::visit(
      // I thought it would be possible to skip the overloaded (just like
      // in BaseFactToDLIR, but I needed to give the types for each
      // variant explicitly to avoid a type error).
      raksha::utils::overloaded{
          [this, &speaker](const Fact& fact) {
            return GenerateDLIRAssertions(speaker, fact);
          },
          [this, &speaker](const ConditionalAssertion& conditional_assertion) {
            return GenerateDLIRAssertions(speaker, conditional_assertion);
          }},
      assertion.GetValue());
}

std::vector<DLIRAssertion> LoweringToDatalogPass::SaysAssertionToDLIR(
    const SaysAssertion& says_assertion) {
  std::vector<DLIRAssertion> ret = {};
  for (const Assertion& assertion : says_assertion.assertions()) {
    std::vector<DLIRAssertion> single_translation =
        SingleSaysAssertionToDLIR(says_assertion.principal(), assertion);
    MoveAppend(ret, std::move(single_translation));
  }
  return ret;
}

std::vector<DLIRAssertion> LoweringToDatalogPass::SaysAssertionsToDLIR(
    const std::vector<SaysAssertion>& says_assertions) {
  std::vector<DLIRAssertion> ret = {};
  for (const SaysAssertion& says_assertion : says_assertions) {
    auto single_translation = SaysAssertionToDLIR(says_assertion);
    MoveAppend(ret, std::move(single_translation));
  }
  return ret;
}

std::vector<DLIRAssertion> LoweringToDatalogPass::QueriesToDLIR(
    const std::vector<Query>& queries) {
  return MapIter<Query, DLIRAssertion>(queries, [this](const Query& query) {
    auto [main_pred, not_used] = FactToDLIR(query.principal(), query.fact());
    main_pred = PushPrincipal("says_", query.principal(), main_pred);
    Predicate lhs(query.name(), {"dummy_var"}, kPositive);
    return DLIRAssertion(DLIRCondAssertion(lhs, {main_pred, kDummyPredicate}));
  });
}

DLIRProgram LoweringToDatalogPass::ProgToDLIR(const Program& program) {
  auto dlir_assertions = SaysAssertionsToDLIR(program.says_assertions());
  auto dlir_queries = QueriesToDLIR(program.queries());
  // We need to add a fact that says the dummy variable used in queries is
  // grounded.
  DLIRAssertion dummy_assertion(kDummyPredicate);
  MoveAppend(dlir_assertions, std::move(dlir_queries));
  dlir_assertions.push_back(dummy_assertion);

  auto outputs = MapIter<Query, std::string>(
      program.queries(), [](const Query& query) { return query.name(); });
  return DLIRProgram(dlir_assertions, outputs);
}

}  // namespace raksha::ir::auth_logic
