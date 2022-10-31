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

namespace {
// Generates a new mapping from relation names to relation declarations
// implemented as a flat_hash_map. It is used in the implementation of
// the constructor for DeclarationEnvironment.
using DeclarationMapType =
    common::containers::HashMap<std::string, datalog::RelationDeclaration>;
class RelationDeclarationEnvironmentVisitor
    : public AuthLogicAstTraversingVisitor<
          RelationDeclarationEnvironmentVisitor> {
 public:
  RelationDeclarationEnvironmentVisitor() : decl_map_({}){};
  DeclarationMapType decl_map() { return decl_map_; }

 private:
  void AddDeclaration(const datalog::RelationDeclaration& rel_decl) {
    absl::string_view rel_name = rel_decl.relation_name();
    if (decl_map_.find(rel_name) == decl_map_.end()) {
      decl_map_.insert({std::string{rel_decl.relation_name()}, rel_decl});
    } else {
      LOG(FATAL) << "Error: found multiple declarations of relation named: "
                 << rel_name;
    }
  }
  DeclarationMapType decl_map_;
  // Note that this visito populates the decl_map_ by side-effects
  // rather than by returning a declaration map because the AddDeclaration
  // function needs access to the complete map that has been built
  // thus far in order to throw errors.
  Unit Visit(const datalog::RelationDeclaration& rel_decl) {
    AddDeclaration(rel_decl);
    return Unit();
  }
};

}  // namespace

DeclarationEnvironment::DeclarationEnvironment(const Program& prog) {
  RelationDeclarationEnvironmentVisitor rel_visitor;
  prog.Accept(rel_visitor);
  inner_map_ = rel_visitor.decl_map();
}

datalog::RelationDeclaration DeclarationEnvironment::GetDeclarationOrFatal(
    absl::string_view relation_name) const {
  auto find_result = inner_map_.find(relation_name);
  CHECK(find_result != inner_map_.end())
      << "could not find declaration for relation " << relation_name;
  return find_result->second;
}

}  // namespace raksha::ir::auth_logic
