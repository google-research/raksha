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

/*
This file contains a translation that populates the universe relations.
Universe relations are facts of the form `isSymbol(x)` that are used to
keep datalog rules grounded and finite. This pattern works by putting facts
of the form `isSymbol(x)` on the RHS of a conditional rule where the variable
`x` would otherwise be ungrounded; then the set of symbols is populated by
adding unconditional facts of the form `isSymbol("specific_symbol").` to
populate these universes.

A problem with using this pattern in the context of authorization logic is that
these universes need to be shared across principals, and this is not
expressable in the sufrace language. Also, manually using this pattern is also
tedius, and makes the code less clear. This translation handles
both of these problems by:
    1) extending the program to implement the universe relation pattern by:
    adding universe predicates to the RHS of rules, and adding facts that add
    elements to universes.
    2) extending the program with facts that use delegation to pass membership
    of universes across principals.

This translation is a compiler pass that operates on the data structure
described in `ast.h` and produces a new instance of the data structure in
`ast.h`. It should come before the translation in `lowering_ast_datalog.h`.
*/

#ifndef SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_
#define SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

class UniverseRelationInsertion {
 public:
  // A map from the names of principal / predicate argument literals
  // (the constants wrapped in quotes) to typings.
  using TypeEnv = absl::flat_hash_map<std::string, datalog::ArgumentType>;
  // A map from predicate names to predicate declarations.
  using DeclEnv =
      absl::flat_hash_map<std::string, datalog::RelationDeclaration>;

  static Program InsertUniverseRelations(const Program& prog) {
    // Generate relation declaration environment
    RelationDeclarationEnvironmentVisitor rel_visitor;
    prog.Accept(rel_visitor);
    DeclEnv decl_env = rel_visitor.decl_env();
    // Generate constant type environment
    TypeEnvironmentGenerationVisitor type_env_visitor(decl_env);
    prog.Accept(type_env_visitor);
    TypeEnv type_env = type_env_visitor.type_env();
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
    auto universe_declarations = GetUniverseDeclarations(prog);
    auto new_declarations = prog.relation_declarations();
    new_declarations.insert(new_declarations.end(),
      universe_declarations.begin(), universe_declarations.end());

    return Program(new_declarations, new_says_assertions,
                   prog.queries());
  }

 private:
  static std::string TypeToUniverseName(datalog::ArgumentType arg_type) {
    switch (arg_type.kind()) {
      case datalog::ArgumentType::Kind::kNumber:
        return "isNumber";
      case datalog::ArgumentType::Kind::kPrincipal:
        return "isPrincipal";
      case datalog::ArgumentType::Kind::kCustom:
        return absl::StrCat("is", arg_type.name());
    }
  }

  static BaseFact MakeUniverseMembershipFact(std::string element_name,
                                             datalog::ArgumentType arg_type) {
    return BaseFact(
        datalog::Predicate(TypeToUniverseName(arg_type),  // universe relation
                           {element_name},  // the element to add to universe
                           datalog::Sign::kPositive));
  }

  static datalog::RelationDeclaration TypeToUniverseDeclaration(
      datalog::ArgumentType arg_type) {
    return datalog::RelationDeclaration(TypeToUniverseName(arg_type), false,
                                        {datalog::Argument("x", arg_type)});
  }

  // Determine if an identifier (which may be an argument of a predicate
  // or a principal name) is a constant or variable.
  // TODO(aferr) it would be better to refactor the AST so that
  // identifiers are a separate AST node with constant and non-constant
  // children determined at parse time rather than here since
  // this information is available at parse time.
  static bool IsNameConstant(const std::string& id) {
    if (id[0] == '"') {
      return true;
    } else {
      // Numeric literals are constants and this is
      // a numeric literal if all the characters are numbers.
      for (const char& c : id) {
        if (!isdigit(c)) return false;
      }
      return true;
    }
  }

  static datalog::ArgumentType GetTypingOrFatal(
      const TypeEnv& type_env, const std::string& argument_name) {
    if (type_env.find(argument_name) == type_env.end()) {
      LOG(FATAL) << "could not find typing for argument" << argument_name;
    } else {
      return type_env.find(argument_name)->second;
    }
  }
    
  static datalog::RelationDeclaration GetDeclarationOrFatal(
    const DeclEnv& decl_env, const std::string& predicate_name) {
    if (decl_env.find(predicate_name) == decl_env.end()) {
      LOG(FATAL) << "could not find declaration for relation" << predicate_name;
    } else {
      return decl_env.find(predicate_name)->second;
    } 
  }

  // This generates a set of says assertions that populate all the
  // universes for all the speakers given the type environment. Essentially,
  // it loops through the list of constant principals, and then all the
  // constants of all types, and then generates a SaysAssertion that
  // adds the constant to the universe for each speaker.
  static std::vector<SaysAssertion> GetUniverseDefiningFacts(
      const TypeEnv& type_env) {
    std::vector<Principal> constant_principals;
    for (const auto& typing : type_env) {
      if (typing.second.kind() == datalog::ArgumentType::Kind::kPrincipal) {
        constant_principals.push_back(Principal(typing.first));
      }
    }

    std::vector<SaysAssertion> universe_defining_says_assertions;
    for (const auto& prin : constant_principals) {
      for (const auto& typing : type_env) {
        universe_defining_says_assertions.push_back(SaysAssertion(
            prin,  // speaker
            {Assertion(Fact({}, MakeUniverseMembershipFact(typing.first,
                                                           typing.second)))}));
      }
    }
    return universe_defining_says_assertions;
  }

  // This visitor populates a mapping (flat_hash_map) of constant (literal)
  // names to their types by looking at the relations to which the literals
  // are applied and declarations of those relations (including the types
  // of the parameters where they are applied). It takes as input
  // a mapping from relation names to relation declarations.
  class TypeEnvironmentGenerationVisitor
      : public AuthLogicAstTraversingVisitor<TypeEnvironmentGenerationVisitor> {
   public:
    // A map from the names of principal / predicate argument literals
    // to typings.
    using TypeEnv = absl::flat_hash_map<std::string, datalog::ArgumentType>;
    // A map from predicate names to predicate declarations.
    using DeclEnv =
        absl::flat_hash_map<std::string, datalog::RelationDeclaration>;

    TypeEnvironmentGenerationVisitor(
        absl::flat_hash_map<std::string, datalog::RelationDeclaration> decl_env)
        : decl_env_(decl_env) {}

    const TypeEnv& type_env() const { return type_env_; }

   private:
    void AddTyping(std::string arg_name, datalog::ArgumentType arg_type) {
      if (!IsNameConstant(arg_name)) {
        // This pass is only meant to find typings for constants,
        // so do nothing if this argument is not a constant.
        return;
      }
      if (type_env_.find(arg_name) == type_env_.end()) {
        // No typing for this constant was found yet,
        // so add it.
        type_env_.insert({arg_name, arg_type});
      } else if (type_env_.find(arg_name)->second != arg_type) {
        // If there is already a typing for this constant,
        // but it is different from the one used here,
        // throw a type error and abort.
        LOG(FATAL) << "type error for constant: " << arg_name;
      }
    }

    // TODO once an AST node for numbers as RVALUEs in numeric
    // comparisons is added, this visitor should also visit
    // the numeric RVALUEs and add them to the type environment
    // with type ArgumentType(kNumber, "Number")
    // (Related to #637).
    // This is a workaround that makes use of the fact that
    // numeric comparisons are represented as predicates
    // with a name that matches the operator:
    // (https://github.com/google-research/raksha/blob/be6ef8e1e1a20735a06637c12db9ed0b87e3d2a2/src/ir/auth_logic/ast_construction.cc#L92)
    static inline bool PredicateIsNumericOperator(const datalog::Predicate& pred) {
      if (pred.name() == "<" || pred.name() == "<" ||
        pred.name() == "=" || pred.name() == "!=" ||
        pred.name() == "<=" || pred.name() == ">=") {
          return true;
      } else {
        return false;
      }
    }

    Unit PreVisit(const Principal& principal) {
      AddTyping(principal.name(), datalog::ArgumentType::MakePrincipalType());
      return Unit();
    }

    Unit Visit(const datalog::Predicate& pred) {
      if (PredicateIsNumericOperator(pred)) {
        // This is part of the workaround for handling numeric comparisons
        // given that the operators use the same AST nodes as predicates.
        for (auto arg : pred.args() ) {
          AddTyping(arg, datalog::ArgumentType::MakeNumberType());
        }
        return Unit();
      } else {
        // This is the case where this is a normal predicate rather
        // than a numeric operator
        datalog::RelationDeclaration decl = GetDeclarationOrFatal(
          decl_env_, pred.name());
        // The relation declarations give the types for each position in the
        // predicate. For the xth argument in the predicate, we want to
        // assign it the type for the xth parameter in the relation declaration.
        // So to do this, we iterate through the list of arguments in the
        // predicate and the list of parameters in the declaration using
        // the indices of the arguments iterator of the predicate.
        for (unsigned i = 0; i < pred.args().size(); i++) {
          AddTyping(pred.args()[i], decl.arguments()[i].argument_type());
        }
        return Unit();
      }
    }

    TypeEnv type_env_;
    DeclEnv decl_env_;
  };

  class RelationDeclarationEnvironmentVisitor
      : public AuthLogicAstTraversingVisitor<
            RelationDeclarationEnvironmentVisitor> {
   public:
    // A map from predicate names to predicate declarations.
    using DeclEnv =
        absl::flat_hash_map<std::string, datalog::RelationDeclaration>;

    const DeclEnv& decl_env() { return decl_env_; }

   private:
    Unit Visit(const datalog::RelationDeclaration& rel_decl) {
      decl_env_.insert({std::string{rel_decl.relation_name()}, rel_decl});
      return Unit();
    }

    DeclEnv decl_env_;
  };

  // This method when given a program returns a set (so the elements
  // are unique) of relation declarations for the universes that are needed
  // based on the types of elements that are mentioned in the original
  // declarations of the program.
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
    UniverseConditionTransformer(const DeclEnv& decl_env)
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
      return SaysAssertion(says_assertion.principal(),
                           std::move(assertion_list));
    }

   private:

    Assertion AddUniverseConditions(const Assertion& assertion) {
      return std::visit(
          [this](const auto& variant) {
            return AddUniverseConditions(variant);
          },
          assertion.GetValue());
    }

    Assertion AddUniverseConditions(const Fact& fact) {
      std::vector<BaseFact> conditions =
          GetUniverseConditions(fact.base_fact());
      if (conditions.size() > 0) {
        return Assertion(ConditionalAssertion(fact, {conditions}));
      } else {
        return Assertion(fact);
      }
    }

    Assertion AddUniverseConditions(
        const ConditionalAssertion& cond_assertion) {
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
      auto decl = GetDeclarationOrFatal(decl_env_, pred.name());
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

    DeclEnv decl_env_;
  };
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_
