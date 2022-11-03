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

#include "src/common/containers/hash_map.h"
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

  const common::containers::HashMap<std::string, datalog::ArgumentType>&
  literal_type_map() const {
    return literal_type_map_;
  }

 private:
  common::containers::HashMap<std::string, datalog::ArgumentType>
      literal_type_map_;
};

};  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_TYPE_ENVIRONMENT_H_
