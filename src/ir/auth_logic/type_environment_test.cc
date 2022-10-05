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

TEST(TypeEnvironmentTests, AddTypingTest1) {
  datalog::Argument param1("param1",
                           datalog::ArgumentType::MakeCustomType("SomeCustom"));
  datalog::RelationDeclaration decl1("relname", false, {param1});
  SaysAssertion saysAssertion1(
      Principal("\"ConstPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  datalog::Argument param2("param2",
                           datalog::ArgumentType::MakePrincipalType());
  datalog::RelationDeclaration decl2("relname2", false, {param2});
  SaysAssertion saysAssertion2(
      Principal("\"ConstPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname2", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  Program prog({decl1, decl2}, {saysAssertion1, saysAssertion2}, {});
  DeclarationEnvironment decl_env(prog);
  // This is expected to fail because someArgument is used in two places
  // with different types
  EXPECT_DEATH(TypeEnvironment type_env(decl_env, prog),
               "arg_type Type error for constant: \"someArgument\"");
}

TEST(TypeEnvironmentTests, AddTypingTest2) {
  datalog::Argument param1("param1",
                           datalog::ArgumentType::MakeCustomType("SomeCustom"));
  datalog::RelationDeclaration decl1("relname", false, {param1});
  SaysAssertion saysAssertion1(
      Principal("\"ConstPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  datalog::RelationDeclaration decl2("relname2", false, {param1});
  SaysAssertion saysAssertion2(
      Principal("\"OtherPrincipal\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("relname2", {"\"someArgument\""},
                                               datalog::Sign::kPositive))))});
  Program prog({decl1, decl2}, {saysAssertion1, saysAssertion2}, {});
  DeclarationEnvironment decl_env(prog);
  // This test is just meant to check that there is NOT a failure
  // during TypeEnv construction even though someArgument is used
  // in more than one place because it has the same time for all uses
  TypeEnvironment type_env(decl_env, prog);
}

}  // namespace raksha::ir::auth_logic
