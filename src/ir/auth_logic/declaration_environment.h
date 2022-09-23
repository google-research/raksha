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

#ifndef SRC_IR_AUTH_LOGIC_DECLARATION_ENVIRONMENT_H_
#define SRC_IR_AUTH_LOGIC_DECLARATION_ENVIRONMENT_H_

#include "absl/container/flat_hash_map.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

// A map from relation names to relation declarations.
class DeclarationEnvironment {
 public:
  DeclarationEnvironment(const Program& prog);

  void AddDeclaration(const datalog::RelationDeclaration& rel_decl) {
    inner_map_.insert({std::string{rel_decl.relation_name()}, rel_decl});
  }

  datalog::RelationDeclaration GetDeclarationOrFatal(
      absl::string_view relation_name);

 private:
  // Generates a new mapping from relation names to relation declarations
  // implemented as a flat_hash_map. It is used in the implementation of
  // the constructor for DeclarationEnvironment. This is implemented
  // as a nested class so that it can access the DeclarationEnvironment's
  // private state during the constructor.
  class RelationDeclarationEnvironmentVisitor
      : public AuthLogicAstTraversingVisitor<
            RelationDeclarationEnvironmentVisitor> {
   public:
    RelationDeclarationEnvironmentVisitor(DeclarationEnvironment& enclosing_env)
        : enclosing_env_(enclosing_env) {}

   private:
    Unit Visit(const datalog::RelationDeclaration& rel_decl) {
      enclosing_env_.AddDeclaration(rel_decl);
      return Unit();
    }
    DeclarationEnvironment& enclosing_env_;
  };

  absl::flat_hash_map<std::string, datalog::RelationDeclaration> inner_map_;
};

};  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_DECLARATION_ENVIRONMENT_H_
