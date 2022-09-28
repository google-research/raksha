/* Copyright 2022 Google LLC.
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

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

TEST(DeclarationEnvironmentTests, LookupTest) {
  datalog::Argument arg1("arg1", datalog::ArgumentType::MakePrincipalType());
  datalog::RelationDeclaration decl1("relname", false, {arg1});
  datalog::RelationDeclaration decl2("relname2", false, {arg1});
  Program prog({decl1, decl2}, {}, {});
  DeclarationEnvironment decl_env(prog);
  EXPECT_EQ(decl_env.GetDeclarationOrFatal("relname"), decl1);
  EXPECT_EQ(decl_env.GetDeclarationOrFatal("relname2"), decl2);
}

TEST(DeclarationEnvironmentTests, FatalLookupTest) {
  Program prog({}, {}, {});
  DeclarationEnvironment decl_env(prog);
  EXPECT_DEATH(decl_env.GetDeclarationOrFatal("relation_not_here"),
    "could not find declaration for relation relation_not_here");
}

TEST(DeclarationEnvironmentTests, FatalAddTest) {
  datalog::Argument arg1("arg1", datalog::ArgumentType::MakePrincipalType());
  datalog::RelationDeclaration decl1("relname", false, {arg1});
  datalog::RelationDeclaration decl2("relname", false, {arg1});
  Program prog({decl1, decl2}, {}, {});
  EXPECT_DEATH(DeclarationEnvironment decl_env(prog), "Error: found multiple declarations of relation named: relname");
}

}  // namespace raksha::ir::auth_logic
