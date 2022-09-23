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

#include "absl/container/flat_hash_map.h"

#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/auth_logic/declaration_environment.h"
#include "src/ir/datalog/program.h"

#ifndef SRC_IR_AUTH_LOGIC_TYPE_ENVIRONMENT_H_
#define SRC_IR_AUTH_LOGIC_TYPE_ENVIRONMENT_H_

namespace raksha::ir::auth_logic {

// A map from the names of principal / predicate argument literals
// (the constants wrapped in quotes) to typings.
class TypeEnvironment {
 public:
  TypeEnvironment(DeclarationEnvironment decl_env, const Program& prog);

  datalog::ArgumentType GetTypingOrFatal(absl::string_view argument_name);

  const absl::flat_hash_map<std::string, datalog::ArgumentType>& inner_map()
      const {
    return inner_map_;
  }

 private:
  void AddTyping(absl::string_view arg_name, datalog::ArgumentType arg_type);

  // This visitor aids in the construction of the TypeEnvironment.
  // It populates a mapping (flat_hash_map) of constant (literal)
  // names to their types by looking at the relations to which the literals
  // are applied and declarations of those relations (including the types
  // of the parameters where they are applied). It takes as input
  // a DeclarationEnvironment.
  // This is a nested class because it needs to access the private
  // fields of the TypeEnvironment in the constructor implementation.
  class TypeEnvironmentGenerationVisitor
      : public AuthLogicAstTraversingVisitor<TypeEnvironmentGenerationVisitor> {
   public:
    TypeEnvironmentGenerationVisitor(DeclarationEnvironment decl_env,
                                     TypeEnvironment& enclosing_env)
        : decl_env_(decl_env), enclosing_env_(enclosing_env) {}

   private:
    Unit PreVisit(const Principal& principal);
    Unit Visit(const datalog::Predicate& pred);
    DeclarationEnvironment decl_env_;
    TypeEnvironment& enclosing_env_;
  };

  absl::flat_hash_map<std::string, datalog::ArgumentType> inner_map_;
};

};  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_TYPE_ENVIRONMENT_H_