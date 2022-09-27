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

#include "src/ir/auth_logic/declaration_environment.h"

namespace raksha::ir::auth_logic {

DeclarationEnvironment::DeclarationEnvironment(const Program& prog) {
  RelationDeclarationEnvironmentVisitor rel_visitor(*this);
  prog.Accept(rel_visitor);
}

datalog::RelationDeclaration DeclarationEnvironment::GetDeclarationOrFatal(
    absl::string_view relation_name) const {
  auto find_result = inner_map_.find(relation_name);
  CHECK(find_result != inner_map_.end())
      << "could not find declaration for relation " << relation_name;
  return find_result->second;
}

}  // namespace raksha::ir::auth_logic
