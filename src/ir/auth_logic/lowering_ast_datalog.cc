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

#include <iostream>

#include "absl/algorithm/container.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

namespace {
// This function takes a `predicate` and creates a new predicate as follows:
//   - Prepend the `modifier` to the name of the given `predicate`.
//   - Prepend the given `args` to the original list of arguments of
//   `predicate`.
//
// This is used in a few places in the translation, for example, to translate
// "X says blah(args)" into "says_blah(X, args)".
datalog::Predicate PushOntoPredicate(absl::string_view modifier,
                                     std::vector<std::string> new_args,
                                     const datalog::Predicate& predicate) {
  std::string new_name = absl::StrCat(std::move(modifier), predicate.name());
  absl::c_copy(predicate.args(), std::back_inserter(new_args));
  datalog::Sign sign_copy = predicate.sign();
  return datalog::Predicate(new_name, std::move(new_args), sign_copy);
}

// This function is an abbreviation for `PushPrincipal` where:
// - a modifier is added
// - just one new principal is added as an argument.
// This is a common case in this translation because it is used for
// `x says blah(args)` and `x canActAs y` and other constructions involving a
// principal name.
datalog::Predicate PushPrincipal(absl::string_view modifier,
                                 const Principal& principal,
                                 const datalog::Predicate& predicate) {
  return PushOntoPredicate(modifier, {principal.name()}, predicate);
}

datalog::Predicate AttributeToDLIR(const Attribute& attribute) {
  // If attribute is `X pred(args...)` the following predicate is
  // `pred(X, args...)`
  return PushPrincipal("", attribute.principal(), attribute.predicate());
}

datalog::Predicate CanActAsToDLIR(const CanActAs& can_act_as) {
  return datalog::Predicate(
      "canActAs",
      {can_act_as.left_principal().name(), can_act_as.right_principal().name()},
      datalog::kPositive);
}
}  // namespace

void LoweringToDatalogPass::update_can_say_depth(
    std::string_view predicate_name, uint64_t depth) {
  if (can_say_depth.find(predicate_name) == can_say_depth.end()) {
    can_say_depth.insert({predicate_name, depth});
  } else if (depth > can_say_depth.find(predicate_name)->second) {
    can_say_depth.find(predicate_name)->second = depth;
  }
}

datalog::Rule LoweringToDatalogPass::SpokenAttributeToDLIR(
    const Principal& speaker, const Attribute& attribute) {
  // Attributes interact with "canActAs" because if "Y canActAs X"
  // then Y also picks up X's attributes. We need to generate
  // an additional rule to implement this behavior. If the attribute
  // under translation is `X PredX`, the additional rule is:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X PredX`
  // (Where Y is a fresh variable)
  Principal prin_y(FreshVar());

  // This is `speaker says Y PredX`
  datalog::Predicate y_predX =
      AttributeToDLIR(Attribute(prin_y, attribute.predicate()));
  datalog::Predicate generated_lhs = PushPrincipal("says_", speaker, y_predX);

  // This is `speaker says Y canActAs X`
  datalog::Predicate y_can_act_as_x =
      CanActAsToDLIR(CanActAs(prin_y, attribute.principal()));

  // This is `speaker says X PredX`
  datalog::Predicate main_predicate = AttributeToDLIR(attribute);

  // This is the full generated rule:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X PredX`
  return datalog::Rule(generated_lhs,
                       {PushPrincipal("says_", speaker, y_can_act_as_x),
                        PushPrincipal("says_", speaker, main_predicate)});
}

datalog::Rule LoweringToDatalogPass::SpokenCanActAsToDLIR(
    const Principal& speaker, const CanActAs& can_act_as) {
  // "canActAs" facts are passed to principals via other canActAs facts in
  // essentially the same way as attributes. This function adds extra
  // rules to pass these around. If the `canActAs` under translation
  // is `X canActAs Z`, then the rule we need to generate is:
  // `speaker says Y canActAs Z :-
  //    speaker says Y canActAs X, speaker says X canActAs Z`
  // (Where Y is a fresh variable)
  Principal prin_y(FreshVar());
  // LoweringToDatalogPass::update_can_say_depth();
  // This is `speaker says Y canActAs Z`
  datalog::Predicate y_can_act_as_z =
      CanActAsToDLIR(CanActAs(prin_y, can_act_as.right_principal()));
  datalog::Predicate generated_lhs =
      PushPrincipal("says_", speaker, y_can_act_as_z);

  // This is `speaker says Y canActAs X`
  datalog::Predicate y_can_act_as_x =
      CanActAsToDLIR(CanActAs(prin_y, can_act_as.left_principal()));

  // This is `speaker says X canActAs Z`
  datalog::Predicate main_predicate = CanActAsToDLIR(can_act_as);

  // This is the full generated rule:
  // `speaker says Y PredX :-
  //    speaker says Y canActAs X, speaker says X canActAs Z`
  return datalog::Rule(generated_lhs,
                       {PushPrincipal("says_", speaker, y_can_act_as_x),
                        PushPrincipal("says_", speaker, main_predicate)});
}

std::pair<datalog::Predicate, std::vector<datalog::Rule>>
LoweringToDatalogPass::BaseFactToDLIRInner(
    const Principal& speaker, const datalog::Predicate& predicate) {
  return std::make_pair(predicate, std::vector<datalog::Rule>({}));
}

std::pair<datalog::Predicate, std::vector<datalog::Rule>>
LoweringToDatalogPass::BaseFactToDLIRInner(const Principal& speaker,
                                           const Attribute& attribute) {
  return std::make_pair(
      AttributeToDLIR(attribute),
      std::vector<datalog::Rule>({SpokenAttributeToDLIR(speaker, attribute)}));
}

std::pair<datalog::Predicate, std::vector<datalog::Rule>>
LoweringToDatalogPass::BaseFactToDLIRInner(const Principal& speaker,
                                           const CanActAs& canActAs) {
  return std::make_pair(
      CanActAsToDLIR(canActAs),
      std::vector<datalog::Rule>({SpokenCanActAsToDLIR(speaker, canActAs)}));
}

std::pair<datalog::Predicate, std::vector<datalog::Rule>>
LoweringToDatalogPass::BaseFactToDLIR(const Principal& speaker,
                                      const BaseFact& base_fact) {
  return std::visit(
      [this, &speaker](auto value) {
        return BaseFactToDLIRInner(speaker, value);
      },
      base_fact.GetValue());
}

std::pair<datalog::Predicate, std::vector<datalog::Rule>>
LoweringToDatalogPass::FactToDLIR(const Principal& speaker, const Fact& fact) {
  auto [inner_fact_dlir, gen_rules] = BaseFactToDLIR(speaker, fact.base_fact());
  uint64_t can_say_depth_ = 0;
  for (const Principal& delegatees : fact.delegation_chain()) {
    Principal fresh_principal(FreshVar());
    // The following code generates the extra rule that does
    // delegation. This rule is:
    // ```
    // speaker says inner_fact_dlir:-
    //      fresh_principal says inner_fact_dlir,
    //      speaker says fresh_principal canSay inner_fact_dlir
    // ```

    // This is p says inner_fact_dlir
    datalog::Predicate lhs = PushPrincipal("says_", speaker, inner_fact_dlir);
    datalog::Predicate fresh_prin_says_inner =
        PushPrincipal("says_", fresh_principal, inner_fact_dlir);
    datalog::Predicate speaker_says_fresh_cansay_inner = PushPrincipal(
        "says_", speaker,
        PushPrincipal("canSay_", fresh_principal, inner_fact_dlir));
    std::vector<datalog::Predicate> rhs = {fresh_prin_says_inner,
                                           speaker_says_fresh_cansay_inner};
    gen_rules.push_back(datalog::Rule(lhs, rhs));

    // Note that prin_cansay_pred does not begin with "speaker says"
    // because only the top-level fact should. This could
    // be a recursive call, so it might not be processing
    // the top-level fact. The top-level fact gets wrapped
    // in a "says" during the call to translate the assertion
    // in which this appears.
    datalog::Predicate prin_cansay_pred =
        PushPrincipal("canSay_", delegatees, inner_fact_dlir);
    inner_fact_dlir = prin_cansay_pred;
    can_say_depth_ += 1;
  }
  update_can_say_depth(inner_fact_dlir.name(), can_say_depth_);
  return std::make_pair(inner_fact_dlir, gen_rules);
}

std::vector<datalog::Rule> LoweringToDatalogPass::GenerateDLIRAssertions(
    const Principal& speaker, const Fact& fact) {
  auto [fact_predicate, generated_rules] = FactToDLIR(speaker, fact);
  generated_rules.push_back(
      datalog::Rule(PushPrincipal("says_", speaker, fact_predicate)));
  return generated_rules;
}

std::vector<datalog::Rule> LoweringToDatalogPass::GenerateDLIRAssertions(
    const Principal& speaker,
    const ConditionalAssertion& conditional_assertion) {
  auto dlir_rhs = utils::MapIter<datalog::Predicate>(
      conditional_assertion.rhs(), [this, speaker](const BaseFact& base_fact) {
        auto [dlir_translation, not_used] = BaseFactToDLIR(speaker, base_fact);
        return PushPrincipal("says_", speaker, dlir_translation);
      });
  auto [dlir_lhs, gen_rules] = FactToDLIR(speaker, conditional_assertion.lhs());
  auto dlir_lhs_prime = PushPrincipal("says_", speaker, dlir_lhs);
  gen_rules.push_back(datalog::Rule(dlir_lhs_prime, dlir_rhs));
  return gen_rules;
}

std::vector<datalog::Rule> LoweringToDatalogPass::SingleSaysAssertionToDLIR(
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

std::vector<datalog::Rule> LoweringToDatalogPass::SaysAssertionToDLIR(
    const SaysAssertion& says_assertion) {
  std::vector<datalog::Rule> ret = {};
  for (const Assertion& assertion : says_assertion.assertions()) {
    absl::c_move(
        SingleSaysAssertionToDLIR(says_assertion.principal(), assertion),
        std::back_inserter(ret));
  }
  return ret;
}

std::vector<datalog::Rule> LoweringToDatalogPass::SaysAssertionsToDLIR(
    const std::vector<SaysAssertion>& says_assertions) {
  std::vector<datalog::Rule> ret = {};
  for (const SaysAssertion& says_assertion : says_assertions) {
    absl::c_move(SaysAssertionToDLIR(says_assertion), std::back_inserter(ret));
  }
  return ret;
}

std::vector<datalog::Rule> LoweringToDatalogPass::QueriesToDLIR(
    const std::vector<Query>& queries) {
  return utils::MapIter<datalog::Rule>(queries, [this](const Query& query) {
    auto [main_pred, not_used] = FactToDLIR(query.principal(), query.fact());
    main_pred = PushPrincipal("says_", query.principal(), main_pred);
    datalog::Predicate lhs(query.name(), {R"("dummy_var")"},
                           datalog::kPositive);
    return datalog::Rule(lhs, {main_pred, kDummyPredicate});
  });
}

std::vector<datalog::RelationDeclaration>
LoweringToDatalogPass::RelationDeclarationToDLIR(
    const std::vector<datalog::RelationDeclaration>& relation_declarations) {
  // Note that the additional relation declarations that are generated for
  // (possibly nested) cansay expressions are based on an environment
  // that tracks the maximum depth with which they appear. This environment is
  // populated by side-effect while translating the program body. As a result,
  // it is assumed that this function is called after the body is translated.

  // The base relation declarations are the same as the ones from the
  // surface program plus another one for "canActAs"
  std::vector<datalog::RelationDeclaration> base_declarations =
      relation_declarations;
  base_declarations.push_back(datalog::RelationDeclaration(
      "canActAs", false,
      {datalog::Argument(
           "p1", datalog::ArgumentType(datalog::ArgumentType::Kind::kPrincipal,
                                       "Principal")),
       datalog::Argument(
           "p2", datalog::ArgumentType(datalog::ArgumentType::Kind::kPrincipal,
                                       "Principal"))}));
  // Attributes have a principal that the attribute is applied to
  // as an additional parameter at the beginning of the param list.
  // Transform the attributes
  std::vector<datalog::RelationDeclaration> transformed_declarations =
      utils::MapIter<datalog::RelationDeclaration>(
          base_declarations, [](datalog::RelationDeclaration declaration) {
            if (declaration.is_attribute()) {
              std::vector<datalog::Argument> transformed_arguments = {
                  datalog::Argument("attribute_prin",
                                    datalog::ArgumentType(
                                        datalog::ArgumentType::Kind::kPrincipal,
                                        "Principal"))};
              transformed_arguments.insert(transformed_arguments.end(),
                                           declaration.arguments().begin(),
                                           declaration.arguments().end());
              return datalog::RelationDeclaration(declaration.relation_name(),
                                                  false, transformed_arguments);
            } else {
              return declaration;
            }
          });
  // Produce a mapping from predicate names to predicate typings
  // (where a predicate typing is the same as a predicate relation declaration)
  absl::flat_hash_map<std::string_view, datalog::RelationDeclaration>
      type_environment;
  for (datalog::RelationDeclaration declaration : transformed_declarations) {
    type_environment.insert({declaration.relation_name(), declaration});
  }

  // Generate relation declarations for delegated predicates with
  // each encountered delegation depth
  std::vector<datalog::RelationDeclaration> can_say_declarations;
  for (const auto& can_say_depth_pair : can_say_depth) {
    auto type_iterator = type_environment.find(can_say_depth_pair.first);

    if ((can_say_depth_pair.second > 0) &&
        (type_iterator != type_environment.end())) {
      datalog::RelationDeclaration declaration = type_iterator->second;
      for (uint64_t i = 1; i < can_say_depth_pair.second + 1; ++i) {
        std::vector<datalog::Argument> argument = {datalog::Argument(
            absl::StrCat("delegatee", i),
            datalog::ArgumentType(datalog::ArgumentType::Kind::kPrincipal,
                                  "Principal"))};
        argument.insert(argument.end(), declaration.arguments().begin(),
                        declaration.arguments().end());
        can_say_declarations.push_back(datalog::RelationDeclaration(
            absl::StrCat("canSay_", declaration.relation_name()), false,
            argument));
        declaration = datalog::RelationDeclaration(
            absl::StrCat("canSay_", declaration.relation_name()), false,
            argument);
      }
    }
  }
  transformed_declarations.insert(transformed_declarations.end(),
                                  can_say_declarations.begin(),
                                  can_say_declarations.end());
  // The translated declarations are all extended with "says_" and a speaker
  // argument
  std::vector<datalog::RelationDeclaration>
      says_extended_relational_declarations;
  for (datalog::RelationDeclaration declaration : transformed_declarations) {
    std::vector<datalog::Argument> arguments = {datalog::Argument(
        "speaker", datalog::ArgumentType(
                       datalog::ArgumentType::Kind::kPrincipal, "Principal"))};
    arguments.insert(arguments.end(), declaration.arguments().begin(),
                     declaration.arguments().end());
    says_extended_relational_declarations.push_back(
        datalog::RelationDeclaration(
            absl::StrCat("says_", declaration.relation_name()), false,
            arguments));
  }
  return says_extended_relational_declarations;
}

std::vector<datalog::RelationDeclaration>
LoweringToDatalogPass::QueryRelationDeclarationToDLIR(
    const std::vector<Query>& queries) {
  std::vector<datalog::RelationDeclaration> query_declarations =
      utils::MapIter<datalog::RelationDeclaration>(queries, [](Query query) {
        return datalog::RelationDeclaration(
            query.name(), false,
            {datalog::Argument(
                "dummy_param",
                datalog::ArgumentType(datalog::ArgumentType::Kind::kCustom,
                                      "DummyType"))});
      });
  query_declarations.push_back(datalog::RelationDeclaration(
      "grounded_dummy", false,
      {datalog::Argument(
          "dummy_param",
          datalog::ArgumentType(datalog::ArgumentType::Kind::kCustom,
                                "DummyType"))}));
  return query_declarations;
}

datalog::Program LoweringToDatalogPass::ProgToDLIR(const Program& program) {
  auto dlir_assertions = SaysAssertionsToDLIR(program.says_assertions());
  // We need to add a fact that says the dummy variable used in queries is
  // grounded.
  datalog::Rule dummy_assertion(kDummyPredicate);
  absl::c_move(QueriesToDLIR(program.queries()),
               std::back_inserter(dlir_assertions));
  dlir_assertions.push_back(dummy_assertion);

  auto outputs = utils::MapIter<std::string>(
      program.queries(), [](const Query& query) { return query.name(); });

  // Note that the additional relation declarations that are generated for
  // (possibly nested) cansay expressions are based on an environment
  // that tracks the maximum depth with which they appear. This environment is
  // populated by side-effect while translating the program body. As a result,
  // it is assumed that this function is called after the body is translated.
  auto dlir_relation_declarations =
      RelationDeclarationToDLIR(program.relation_declarations());
  auto dlir_relation_declarations_queries =
      QueryRelationDeclarationToDLIR(program.queries());
  dlir_relation_declarations.insert(dlir_relation_declarations.end(),
                                    dlir_relation_declarations_queries.begin(),
                                    dlir_relation_declarations_queries.end());

  return datalog::Program(dlir_relation_declarations, dlir_assertions, outputs);
}

}  // namespace raksha::ir::auth_logic
