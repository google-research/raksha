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

namespace raksha::ir::auth_logic {

namespace {
// Determine if an identifier (which may be an argument of a predicate
// or a principal name) is a constant or variable.
// TODO(#673 aferr) it would be better to refactor the AST so that
// identifiers are a separate AST node with constant and non-constant
// children determined at parse time rather than here since
// this information is available at parse time.
bool IsNameConstant(absl::string_view id) {
  if (id[0] == '"') {
    return true;
  } else {
    // Numeric literals are constants and this is
    // a numeric literal if all the characters are numbers.
    for (char c : id) {
      if (!isdigit(c)) return false;
    }
    return true;
  }
}

// (TODO: #672) once an AST node for numbers as RVALUEs in numeric
// comparisons is added, this visitor should also visit
// the numeric RVALUEs and add them to the type environment
// with type ArgumentType(kNumber, "Number")
// This is a workaround that makes use of the fact that
// numeric comparisons are represented as predicates
// with a name that matches the operator:
// (https://github.com/google-research/raksha/blob/be6ef8e1e1a20735a06637c12db9ed0b87e3d2a2/src/ir/auth_logic/ast_construction.cc#L92)
static inline bool PredicateIsNumericOperator(const datalog::Predicate& pred) {
  if (pred.name() == "<" || pred.name() == "<" || pred.name() == "=" ||
      pred.name() == "!=" || pred.name() == "<=" || pred.name() == ">=") {
    return true;
  } else {
    return false;
  }
}

std::string TypeToUniverseName(datalog::ArgumentType arg_type) {
  switch (arg_type.kind()) {
    case datalog::ArgumentType::Kind::kNumber:
      return "isNumber";
    case datalog::ArgumentType::Kind::kPrincipal:
      return "isPrincipal";
    case datalog::ArgumentType::Kind::kCustom:
      return absl::StrCat("is", arg_type.name());
  }
}

BaseFact MakeUniverseMembershipFact(std::string element_name,
                                    datalog::ArgumentType arg_type) {
  return BaseFact(
      datalog::Predicate(TypeToUniverseName(arg_type),  // universe relation
                         {element_name},  // the element to add to universe
                         datalog::Sign::kPositive));
}

datalog::RelationDeclaration TypeToUniverseDeclaration(
    datalog::ArgumentType arg_type) {
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
  for (const auto& typing : type_env.inner_map()) {
    if (typing.second.kind() == datalog::ArgumentType::Kind::kPrincipal) {
      constant_principals.push_back(Principal(typing.first));
    }
  }

  std::vector<SaysAssertion> universe_defining_says_assertions;
  for (const auto& prin : constant_principals) {
    for (const auto& typing : type_env.inner_map()) {
      universe_defining_says_assertions.push_back(SaysAssertion(
          prin,  // speaker
          {Assertion(Fact(
              {}, MakeUniverseMembershipFact(typing.first, typing.second)))}));
    }
  }
  return universe_defining_says_assertions;
}

// This visitor is used only to implmenet GetUniverseDeclarations
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
      universe_declarations.emplace(
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
      TypeToUniverseDeclaration(datalog::ArgumentType(
          datalog::ArgumentType::Kind::kNumber, "Number")),
      TypeToUniverseDeclaration(datalog::ArgumentType(
          datalog::ArgumentType::Kind::kPrincipal, "Principal")),
  };
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
// Internally, it extends a visitor for a part of this translation
class UniverseConditionTransformer
    : public AuthLogicAstTraversingVisitor<UniverseConditionTransformer,
                                           std::vector<BaseFact>> {
 public:
  UniverseConditionTransformer(const DeclarationEnvironment& decl_env)
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
    std::vector<BaseFact> conditions =
        GetUniverseConditions(cond_assertion.lhs().base_fact());
    absl::c_copy(cond_assertion.rhs(), std::back_inserter(conditions));
    return Assertion(ConditionalAssertion(cond_assertion.lhs(), conditions));
  }

  // This method, given a BaseFact appearing in the LHS of an assertion,
  // returns a vector of conditions that should be added to the RHS of that
  // assertion. This method is where this class is actually used as a visitor.
  // The visitor works only on subtrees of the AST that are
  // these BaseFacts on the LHS and returns a set of BaseFacts with
  // the universe conditions.
  inline std::vector<BaseFact> GetUniverseConditions(
      const BaseFact& base_fact) {
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
    for (unsigned i = 0; i < pred.args().size(); i++) {
      auto arg = pred.args()[i];
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

DeclarationEnvironment::DeclarationEnvironment(const Program& prog) {
  RelationDeclarationEnvironmentVisitor rel_visitor(*this);
  prog.Accept(rel_visitor);
}

datalog::RelationDeclaration DeclarationEnvironment::GetDeclarationOrFatal(
    absl::string_view relation_name) {
  if (inner_map_.find(relation_name) == inner_map_.end()) {
    LOG(FATAL) << "could not find declaration for relation" << relation_name;
  } else {
    return inner_map_.find(relation_name)->second;
  }
}

TypeEnvironment::TypeEnvironment(DeclarationEnvironment decl_env,
                                 const Program& prog) {
  TypeEnvironmentGenerationVisitor type_gen_visitor(decl_env, *this);
  prog.Accept(type_gen_visitor);
}

datalog::ArgumentType TypeEnvironment::GetTypingOrFatal(
    absl::string_view argument_name) {
  auto find_result = inner_map_.find(argument_name);
  CHECK(find_result != inner_map_.end())
      << "Could not find typing for argument " << argument_name;
  return find_result->second;
}

void TypeEnvironment::AddTyping(std::string arg_name,
                                datalog::ArgumentType arg_type) {
  if (!IsNameConstant(arg_name)) {
    // This pass is only meant to find typings for constants,
    // so do nothing if this argument is not a constant.
    return;
  }
  auto insert_result = inner_map_.insert({arg_name, arg_type});
  CHECK(insert_result.second)
      << "Type error for constant: " << insert_result.first->first;
}

Unit TypeEnvironment::TypeEnvironmentGenerationVisitor::PreVisit(
    const Principal& principal) {
  enclosing_env_.AddTyping(principal.name(),
                           datalog::ArgumentType::MakePrincipalType());
  return Unit();
}

Unit TypeEnvironment::TypeEnvironmentGenerationVisitor::Visit(
    const datalog::Predicate& pred) {
  if (PredicateIsNumericOperator(pred)) {
    // This is part of the workaround for handling numeric comparisons
    // given that the operators use the same AST nodes as predicates.
    for (auto arg : pred.args()) {
      enclosing_env_.AddTyping(arg, datalog::ArgumentType::MakeNumberType());
    }
    return Unit();
  } else {
    // This is the case where this is a normal predicate rather
    // than a numeric operator
    datalog::RelationDeclaration decl =
        decl_env_.GetDeclarationOrFatal(pred.name());
    // The relation declarations give the types for each position in the
    // predicate. For the xth argument in the predicate, we want to
    // assign it the type for the xth parameter in the relation declaration.
    // So to do this, we iterate through the list of arguments in the
    // predicate and the list of parameters in the declaration using
    // the indices of the arguments iterator of the predicate.
    for (unsigned i = 0; i < pred.args().size(); i++) {
      enclosing_env_.AddTyping(pred.args()[i],
                               decl.arguments()[i].argument_type());
    }
    return Unit();
  }
}

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
  UniverseConditionTransformer universe_condition_transformer(decl_env);
  for (const auto& says_assertion : prog.says_assertions()) {
    new_says_assertions.push_back(
        universe_condition_transformer.AddUniverseConditions(says_assertion));
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