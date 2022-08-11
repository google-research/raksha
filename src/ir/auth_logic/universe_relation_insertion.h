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

#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

class UniverseRelationInsertion {
 public:
  static Program InsertUniverseRelations(const Program& prog) {
    // TODO
    return prog;
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

  // Determine if an identifier (which may be an argument of a predicate
  // or a principal name) is a constant or variable. TODO it may be
  // better to refactor the AST so that "constantness" is determined
  // at parse time rather than here since this information is
  // available at parse time.
  static bool IsNameConstant(std::string &id) {
    // TODO
    return false;
  }

// This class implements one public method that, given a Program,
// returns a set (so the elements are unique) of relation declarations
// for the universes that are needed based on the types of
// elements that are mentioned in the original declarations
// of the program.
  class UniverseDeclarationGenerationVisitor
      : public AuthLogicAstTraversingVisitor<
            UniverseDeclarationGenerationVisitor,
            absl::flat_hash_set<datalog::RelationDeclaration>> {
   public:
    static absl::flat_hash_set<datalog::RelationDeclaration> GetUniverseDeclarations(
        const Program& prog) {
      absl::flat_hash_set<datalog::RelationDeclaration> ret =
          prog.Accept(*this);
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

   private:
    datalog::RelationDeclaration TypeToUniverseDeclaration(
        datalog::ArgumentType arg_type) {
      return datalog::RelationDeclaration(TypeToUniverseName(arg_type), false,
                                          {datalog::Argument("x", arg_type)});
    }

    absl::flat_hash_set<datalog::RelationDeclaration> GetDefaultValue()
        override {
      return {};
    }

    absl::flat_hash_set<datalog::RelationDeclaration> CombineResult(
        absl::flat_hash_set<datalog::RelationDeclaration> left,
        absl::flat_hash_set<datalog::RelationDeclaration> right) override {
      left.merge(std::move(right));
      return left;
    }

    absl::flat_hash_set<datalog::RelationDeclaration> Visit(
        const datalog::RelationDeclaration& relation_declaration) override {
      absl::flat_hash_set<datalog::RelationDeclaration> universe_declarations;
      for (const auto& argument : relation_declaration.arguments()) {
        universe_declarations.emplace(
            TypeToUniverseDeclaration(argument.argument_type()));
      }
      return universe_declarations;
    }
  };
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_
