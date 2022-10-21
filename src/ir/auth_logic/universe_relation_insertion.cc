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

#include "src/ir/auth_logic/universe_relation_insertion.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/auth_logic/declaration_environment.h"
#include "src/ir/auth_logic/is_name_constant.h"
#include "src/ir/auth_logic/type_environment.h"

namespace raksha::ir::auth_logic {

namespace {

std::string TypeToUniverseName(const datalog::ArgumentType& arg_type) {
  switch (arg_type.kind()) {
    case datalog::ArgumentType::Kind::kNumber:
      return "isNumber";
    case datalog::ArgumentType::Kind::kPrincipal:
      return "isPrincipal";
    case datalog::ArgumentType::Kind::kCustom:
      return absl::StrCat("is", arg_type.name());
    default:
      CHECK(false) << "Unexpected variant of datalog argument type";
  }
}

BaseFact MakeUniverseMembershipFact(absl::string_view element_name,
                                    const datalog::ArgumentType& arg_type) {
  return BaseFact(datalog::Predicate(
      TypeToUniverseName(arg_type),  // universe relation
      {std::string(element_name)},   // the element to add to universe
      datalog::Sign::kPositive));
}

datalog::RelationDeclaration TypeToUniverseDeclaration(
    const datalog::ArgumentType& arg_type) {
  return datalog::RelationDeclaration(TypeToUniverseName(arg_type), false,
                                      {datalog::Argument("x", arg_type)});
}

// This generates a set of says assertions that populate all the
// universes for all the speakers given the type environment. Essentially,
// it loops through the list of constant principals, and then all the
// constants of all types, and then generates a SaysAssertion that
// adds the constant to the universe for each speaker.
std::vector<SaysAssertion> GetUniverseDefiningFacts(
    const TypeEnvironment& type_env) {
  std::vector<Principal> constant_principals;
  for (const auto& typing : type_env.literal_type_map()) {
    if (typing.second.kind() == datalog::ArgumentType::Kind::kPrincipal) {
      constant_principals.push_back(Principal(typing.first));
    }
  }

  std::vector<SaysAssertion> universe_defining_says_assertions;
  for (const auto& prin : constant_principals) {
    for (const auto& typing : type_env.literal_type_map()) {
      universe_defining_says_assertions.push_back(SaysAssertion(
          prin,  // speaker
          {Assertion(Fact(
              {}, MakeUniverseMembershipFact(typing.first, typing.second)))}));
    }
  }
  return universe_defining_says_assertions;
}

// This visitor is used only to implement GetUniverseDeclarations
class UniverseDeclarationGenerationVisitor
    : public AuthLogicAstTraversingVisitor<
          UniverseDeclarationGenerationVisitor,
          absl::flat_hash_set<datalog::RelationDeclaration>> {
 private:
  using DeclSet = absl::flat_hash_set<datalog::RelationDeclaration>;
  DeclSet GetDefaultValue() override { return {}; }

  DeclSet CombineResult(DeclSet left, DeclSet right) override {
    left.merge(std::move(right));
    return left;
  }

  DeclSet Visit(
      const datalog::RelationDeclaration& relation_declaration) override {
    DeclSet universe_declarations;
    for (const auto& argument : relation_declaration.arguments()) {
      universe_declarations.insert(
          TypeToUniverseDeclaration(argument.argument_type()));
    }
    return universe_declarations;
  }
};

static absl::flat_hash_set<datalog::RelationDeclaration>
GetUniverseDeclarations(const Program& prog) {
  UniverseDeclarationGenerationVisitor gen;
  absl::flat_hash_set<datalog::RelationDeclaration> ret = prog.Accept(gen);
  // It is possible that universes need to be created
  // for numbers and principals even if they are not
  // referenced in relation declarations in the front-end
  absl::flat_hash_set<datalog::RelationDeclaration> builtin_universes = {
      TypeToUniverseDeclaration(datalog::ArgumentType::MakeNumberType()),
      TypeToUniverseDeclaration(datalog::ArgumentType::MakePrincipalType())};
  ret.merge(builtin_universes);
  return ret;
}

//----------------------------------------------------------------------------
// Grounding Condition Transformation
//----------------------------------------------------------------------------
// This class transforms SaysAssertions so that if
// there are any potentially ungrounded variables on the LHS of an assertion,
// conditions are added to the RHS that check that this variable
// is in a universe. As a way of safely overapproximating the ungrounded
// variables, this just assumes any *non-constant* arguments in the RHS
// are ungrounded, and adds conditions for them to the LHS.
// It implements just one public method,
// `AddUniverseConditions(SaysAssertion says_assertion)`
// that returns a transformed version of the says assertion.
// Internally, it extends a visitor for a part of this translation.
//
// Example transformation 1:
// Input:
// .decl someRelation(x: SomeCustomType)
// "PrinX" says someRelation(anyCustom).
// Output:
// .decl someRelation(x: SomeCustomType)
// .decl isCustom(x: SomeCustomType)
// "PrinX" says someRelation(anyCustom) :- isCustom(anyCustom)
//
// Example transformation 2 (illustrating conservativeness):
// Input:
// .decl someRelation(x: SomeCustomType)
// .decl otherRelation(x: SomeCustomType)
// "PrinX" says someRelation(anyCustom) :- otherRelation(anyCustom).
// Output:
// .decl someRelation(x: SomeCustomType)
// .decl otherRelation(x: SomeCustomType)
// .decl isCustom(x: SomeCustomType)
// "PrinX" says someRelation(anyCustom) :- isCustom(anyCustom),
//             otherRelation(anyCustom).

class GroundingConditionTransformer
    : public AuthLogicAstTraversingVisitor<GroundingConditionTransformer,
                                           std::vector<BaseFact>> {
 public:
  GroundingConditionTransformer(const DeclarationEnvironment& decl_env)
      : decl_env_(decl_env) {}

  // The `AddUniverseConditions` methods, including this one,
  // are not implemented as actual visits because these methods
  // are implementing a transformation, and the returned values
  // need to be placed in a part of the syntactic context that
  // is specific to each syntactic element.
  SaysAssertion AddUniverseConditions(const SaysAssertion& says_assertion) {
    std::vector<Assertion> assertion_list;
    for (const Assertion& assertion : says_assertion.assertions()) {
      assertion_list.push_back(AddUniverseConditions(assertion));
    }
    return SaysAssertion(says_assertion.principal(), std::move(assertion_list));
  }

 private:
  Assertion AddUniverseConditions(const Assertion& assertion) {
    return std::visit(
        [this](const auto& variant) { return AddUniverseConditions(variant); },
        assertion.GetValue());
  }

  Assertion AddUniverseConditions(const Fact& fact) {
    std::vector<BaseFact> conditions = GetUniverseConditions(fact.base_fact());
    if (conditions.size() > 0) {
      return Assertion(ConditionalAssertion(fact, {conditions}));
    } else {
      return Assertion(fact);
    }
  }

  Assertion AddUniverseConditions(const ConditionalAssertion& cond_assertion) {
    Fact lhs = cond_assertion.lhs();
    std::vector<BaseFact> conditions = GetUniverseConditions(lhs.base_fact());
    absl::c_copy(cond_assertion.rhs(), std::back_inserter(conditions));
    return Assertion(ConditionalAssertion(lhs, conditions));
  }

  // This method, given a BaseFact appearing in the LHS of an assertion,
  // returns a vector of conditions that should be added to the RHS of that
  // assertion. This method is where this class is actually used as a visitor.
  // The visitor works only on subtrees of the AST that are
  // these BaseFacts on the LHS and returns a set of BaseFacts with
  // the universe conditions.
  std::vector<BaseFact> GetUniverseConditions(const BaseFact& base_fact) {
    return base_fact.Accept(*this);
  }

  std::vector<BaseFact> GetDefaultValue() override { return {}; }
  std::vector<BaseFact> CombineResult(std::vector<BaseFact> left,
                                      std::vector<BaseFact> right) override {
    left.insert(left.end(), right.begin(), right.end());
    return left;
  }

  std::vector<BaseFact> PreVisit(const Principal& prin) override {
    if (!IsNameConstant(prin.name())) {
      return {MakeUniverseMembershipFact(
          prin.name(), datalog::ArgumentType::MakePrincipalType())};
    } else {
      return {};
    }
  }

  // Add universe conditions for the non-constant arguments
  std::vector<BaseFact> Visit(const datalog::Predicate& pred) override {
    datalog::RelationDeclaration decl =
        decl_env_.GetDeclarationOrFatal(pred.name());
    std::vector<BaseFact> conditions;
    std::vector<std::string> args = pred.args();
    for (size_t i = 0; i < args.size(); i++) {
      absl::string_view arg = args[i];
      if (!IsNameConstant(arg)) {
        conditions.push_back(MakeUniverseMembershipFact(
            arg, decl.arguments()[i].argument_type()));
      }
    }
    return conditions;
  }

  DeclarationEnvironment decl_env_;
};

}  // namespace

Program InsertUniverseRelations(const Program& prog) {
  // Generate declaration environment
  DeclarationEnvironment decl_env(prog);
  // Generate constant type environment
  TypeEnvironment type_env(decl_env, prog);
  // Generate universe populating facts
  std::vector<SaysAssertion> new_says_assertions =
      GetUniverseDefiningFacts(type_env);
  // Transform assertions to include conditions
  // that check the universes
  GroundingConditionTransformer grounding_condition_transformer(decl_env);
  for (const auto& says_assertion : prog.says_assertions()) {
    new_says_assertions.push_back(
        grounding_condition_transformer.AddUniverseConditions(says_assertion));
  }

  // Add declarations for universe relations.

  absl::flat_hash_set<datalog::RelationDeclaration> universe_declarations =
      GetUniverseDeclarations(prog);
  std::vector<datalog::RelationDeclaration> new_declarations =
      prog.relation_declarations();
  new_declarations.insert(new_declarations.end(), universe_declarations.begin(),
                          universe_declarations.end());

  return Program(new_declarations, new_says_assertions, prog.queries());
}

}  // namespace raksha::ir::auth_logic
