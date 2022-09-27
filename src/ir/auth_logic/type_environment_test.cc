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

#include "src/ir/auth_logic/type_environment.h"

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

TEST(TypeEnvironmentTests, GetTypingTest) {
  datalog::Argument param1("param1",
                           datalog::ArgumentType::MakeCustomType("SomeCustom"));
  datalog::RelationDeclaration decl1("relname", false, {param1});
  SaysAssertion saysAssertion1(
      Principal("\"ConstPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  Program prog({decl1}, {saysAssertion1}, {});
  DeclarationEnvironment decl_env(prog);
  TypeEnvironment type_env(decl_env, prog);
  EXPECT_EQ(type_env.GetTypingOrFatal("\"someArgument\""),
            datalog::ArgumentType::MakeCustomType("SomeCustom"));
  EXPECT_EQ(type_env.GetTypingOrFatal("\"ConstPrincipal\""),
            datalog::ArgumentType::MakePrincipalType());
}

TEST(TypeEnvironmentTests, FatalGetTypingTest) {
  Program prog({}, {}, {});
  DeclarationEnvironment decl_env(prog);
  TypeEnvironment type_env(decl_env, prog);
  EXPECT_DEATH(type_env.GetTypingOrFatal("no_argument_here"), "");
}

TEST(TypeEnvironmentTests, AddTypingTest) {
  datalog::Argument param1("param1",
                           datalog::ArgumentType::MakeCustomType("SomeCustom"));
  datalog::RelationDeclaration decl1("relname", false, {param1});
  SaysAssertion saysAssertion1(
      Principal("\"ConstPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  Program prog({decl1}, {saysAssertion1}, {});
  DeclarationEnvironment decl_env(prog);
  TypeEnvironment type_env(decl_env, prog);
  EXPECT_DEATH(type_env.AddTyping(
                   "\"someArgument\"",
                   datalog::ArgumentType::MakeCustomType("SomeOtherCustom")),
               "");
  EXPECT_DEATH(type_env.AddTyping("\"someArgument\"",
                                  datalog::ArgumentType::MakeNumberType()),
               "");
}

}  // namespace raksha::ir::auth_logic
