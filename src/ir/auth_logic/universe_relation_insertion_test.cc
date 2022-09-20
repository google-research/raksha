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


#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/universe_relation_insertion.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::auth_logic {

  Program BuildTestProgram1() {
      // Input:
      // .decl hasToolOutput(app: App, property: Property)
      // "DPTool" says hasToolOutput(app, "DP").
      datalog::RelationDeclaration has_tool_output_decl("hasToolOutput", false, {
        datalog::Argument("app", datalog::ArgumentType::MakeCustomType("App")),
        datalog::Argument("property", datalog::ArgumentType::MakeCustomType("Property")),
      });
      SaysAssertion assertion1(Principal("\"DPTool\""),
        {Assertion(Fact({}, BaseFact(datalog::Predicate("hasToolOutput",  
          {"app", "\"DP\""}, datalog::kPositive))))
        });
      return Program({has_tool_output_decl}, {assertion1}, {});
  }

  Program BuildTestProgram1Expected() {
      // Output:
      // .decl hasToolOutput(someApp: App, property: Property)
      // .decl isApp(someApp: App)
      // "DPTool" says hasToolOutput(anyApp, "DP") :- isApp(anyApp).
      datalog::RelationDeclaration has_tool_output_decl("hasToolOutput", false, {
        datalog::Argument("app", datalog::ArgumentType::MakeCustomType("App")),
        datalog::Argument("property", datalog::ArgumentType::MakeCustomType("Property")),
      });
      datalog::RelationDeclaration is_app_decl("isApp", false, {
        datalog::Argument("x", datalog::ArgumentType::MakeCustomType("App")),
      });
      datalog::RelationDeclaration is_principal_decl("isPrincipal", false, {
        datalog::Argument("x", datalog::ArgumentType::MakePrincipalType())
      });
      datalog::RelationDeclaration is_property_decl("isProperty", false, {
        datalog::Argument("x", datalog::ArgumentType::MakeCustomType("Property")),
      });
      datalog::RelationDeclaration is_number_decl("isNumber", false, {
        datalog::Argument("x", datalog::ArgumentType::MakeNumberType()),
      });
      SaysAssertion assertion1(Principal("\"DPTool\""),
        {Assertion(ConditionalAssertion(
            Fact({}, BaseFact(datalog::Predicate("hasToolOutput",  
              {"app", "\"DP\""}, datalog::kPositive))), // lhs
            {BaseFact(datalog::Predicate("isApp",  
              {"app"}, datalog::kPositive))} // rhs
        ))});
      SaysAssertion dp_universe(Principal("\"\"")
      return Program({has_tool_output_decl,
        is_app_decl, is_property_decl,
        is_principal_decl, is_number_decl}, {assertion1}, {});
  }

  TEST(UniverseRelationInsertionTests, BasicTest) {
    EXPECT_EQ(
      UniverseRelationInsertion::InsertUniverseRelations(
        BuildTestProgram1()).DebugPrint(),
      BuildTestProgram1Expected().DebugPrint());
  }
}