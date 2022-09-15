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

#include "src/ir/auth_logic/universe_relation_insertion.h"

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {
using ::testing::UnorderedElementsAreArray;

// Simple example
Program BuildTestProgram1() {
  // Input:
  // .decl hasToolOutput(app: App, property: Property)
  // "DPTool" says hasToolOutput(app, "DP").
  datalog::RelationDeclaration has_tool_output_decl(
      "hasToolOutput", false,
      {
          datalog::Argument("app",
                            datalog::ArgumentType::MakeCustomType("App")),
          datalog::Argument("property",
                            datalog::ArgumentType::MakeCustomType("Property")),
      });
  SaysAssertion assertion1(
      Principal("\"DPTool\""),
      {Assertion(Fact(
          {}, BaseFact(datalog::Predicate("hasToolOutput", {"app", "\"DP\""},
                                          datalog::kPositive))))});
  return Program({has_tool_output_decl}, {assertion1}, {});
}

Program BuildTestProgram1Expected() {
  // Output:
  // .decl hasToolOutput(someApp: App, property: Property)
  // .decl isApp(x: App)
  // .decl isPrincipal(x: Principal)
  // .decl isProperty(x: Property)
  // .decl isNumber(x: Number)
  // "DPTool" says hasToolOutput(anyApp, "DP") :- isApp(anyApp).
  // "DPTool" says isPrincipal("DPTool").
  // "DPTool" says isProperty("Property").
  datalog::RelationDeclaration has_tool_output_decl(
      "hasToolOutput", false,
      {
          datalog::Argument("app",
                            datalog::ArgumentType::MakeCustomType("App")),
          datalog::Argument("property",
                            datalog::ArgumentType::MakeCustomType("Property")),
      });
  datalog::RelationDeclaration is_app_decl(
      "isApp", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeCustomType("App")),
      });
  datalog::RelationDeclaration is_principal_decl(
      "isPrincipal", false,
      {datalog::Argument("x", datalog::ArgumentType::MakePrincipalType())});
  datalog::RelationDeclaration is_property_decl(
      "isProperty", false,
      {
          datalog::Argument("x",
                            datalog::ArgumentType::MakeCustomType("Property")),
      });
  datalog::RelationDeclaration is_number_decl(
      "isNumber", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeNumberType()),
      });
  SaysAssertion assertion1(
      Principal("\"DPTool\""),
      {Assertion(ConditionalAssertion(
          Fact({},
               BaseFact(datalog::Predicate("hasToolOutput", {"app", "\"DP\""},
                                           datalog::kPositive))),  // lhs
          {BaseFact(datalog::Predicate("isApp", {"app"}, datalog::kPositive))}
          // rhs
          ))});
  SaysAssertion dp_universe(
      Principal("\"DPTool\""),
      {Assertion(Fact({}, BaseFact(datalog::Predicate("isProperty", {"\"DP\""},
                                                      datalog::kPositive))))});
  SaysAssertion prin_universe(
      Principal("\"DPTool\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("isPrincipal", {"\"DPTool\""},
                                               datalog::kPositive))))});
  return Program({has_tool_output_decl, is_app_decl, is_property_decl,
                  is_principal_decl, is_number_decl},
                 {assertion1, dp_universe, prin_universe}, {});
}

// Example with canSay
Program BuildTestProgram2() {
  // Input:
  // .decl expectedHash(app: App, hash: Number)
  // "Verifier" says {
  //       "EndorsementFile" canSay expectedHash("Application", some_hash).
  // }
  datalog::RelationDeclaration expected_hash(
      "expectedHash", false,
      {
          datalog::Argument("app",
                            datalog::ArgumentType::MakeCustomType("App")),
          datalog::Argument("hash", datalog::ArgumentType::MakeNumberType()),
      });
  SaysAssertion assertion1(
      Principal("\"Verifier\""),
      {Assertion(Fact({Principal("\"EndorsementFile\"")},
                      BaseFact(datalog::Predicate(
                          "expectedHash", {"\"SpecificApplication\"", "hash"},
                          datalog::kPositive))))});
  return Program({expected_hash}, {assertion1}, {});
}

Program BuildTestProgram2Expected() {
  // Output:
  // .decl expectedHash(app: App, hash: Number)
  // .decl isApp(x: App)
  // .decl isPrincipal(x: Principal)
  // .decl isNumber(x: Number)
  // "Verifier" says {
  //       "EndorsementFile" canSay expectedHash("Application", some_hash) :-
  //            isNumber(some_hash).
  // }
  // "Verifier" says isPrincipal("Verifier").
  // "Verifier" says isPrincipal("EndorsementFile").
  // "Verifier" says isApp("Application").
  // "EndorsementFile" says isPrincipal("Verifier").
  // "EndorsementFile" says isPrincipal("EndorsementFile").
  // "EndorsementFile" says isApp("Application").
  datalog::RelationDeclaration expected_hash(
      "expectedHash", false,
      {
          datalog::Argument("app",
                            datalog::ArgumentType::MakeCustomType("App")),
          datalog::Argument("hash", datalog::ArgumentType::MakeNumberType()),
      });
  datalog::RelationDeclaration is_app_decl(
      "isApp", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeCustomType("App")),
      });
  datalog::RelationDeclaration is_principal_decl(
      "isPrincipal", false,
      {datalog::Argument("x", datalog::ArgumentType::MakePrincipalType())});
  datalog::RelationDeclaration is_number_decl(
      "isNumber", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeNumberType()),
      });
  SaysAssertion assertion1(
      Principal("\"Verifier\""),
      {Assertion(ConditionalAssertion(
          Fact({Principal("\"EndorsementFile\"")},
               BaseFact(datalog::Predicate("expectedHash",
                                           {"\"SpecificApplication\"", "hash"},
                                           datalog::kPositive))),  // lhs
          {BaseFact(datalog::Predicate("isNumber", {"hash"},
                                       datalog::kPositive))}  // rhs
          ))});
  SaysAssertion verif_universe_verif(
      Principal("\"Verifier\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("isPrincipal", {"\"Verifier\""},
                                               datalog::kPositive))))});
  SaysAssertion verif_universe_endorsement(
      Principal("\"Verifier\""),
      {Assertion(Fact({}, BaseFact(datalog::Predicate("isPrincipal",
                                                      {"\"EndorsementFile\""},
                                                      datalog::kPositive))))});
  SaysAssertion verif_universe_app(
      Principal("\"Verifier\""),
      {Assertion(Fact(
          {}, BaseFact(datalog::Predicate("isApp", {"\"SpecificApplication\""},
                                          datalog::kPositive))))});
  SaysAssertion endorsement_universe_verif(
      Principal("\"EndorsementFile\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("isPrincipal", {"\"Verifier\""},
                                               datalog::kPositive))))});
  SaysAssertion endorsement_universe_endorsement(
      Principal("\"EndorsementFile\""),
      {Assertion(Fact({}, BaseFact(datalog::Predicate("isPrincipal",
                                                      {"\"EndorsementFile\""},
                                                      datalog::kPositive))))});
  SaysAssertion endorsement_universe_app(
      Principal("\"EndorsementFile\""),
      {Assertion(Fact(
          {}, BaseFact(datalog::Predicate("isApp", {"\"SpecificApplication\""},
                                          datalog::kPositive))))});
  return Program(
      {expected_hash, is_app_decl, is_principal_decl, is_number_decl},
      {assertion1, verif_universe_verif, verif_universe_endorsement,
       verif_universe_app, endorsement_universe_verif,
       endorsement_universe_endorsement, endorsement_universe_app},
      {});
}
// Example with condition
Program BuildTestProgram3() {
  // Input:
  // .decl foo(bar: Bar)
  // "PrinA" says foo(bar1) :- foo(bar2)
  datalog::RelationDeclaration foo(
      "foo", false,
      {
          datalog::Argument("bar",
                            datalog::ArgumentType::MakeCustomType("Bar")),
      });
  SaysAssertion assertion1(
      Principal("\"PrinA\""),
      {Assertion(ConditionalAssertion(
          Fact({}, BaseFact(datalog::Predicate("foo", {"bar1"},
                                               datalog::kPositive))),  // lhs
          {BaseFact(datalog::Predicate("foo", {"bar2"}, datalog::kPositive))}
          // rhs
          ))});
  return Program({foo}, {assertion1}, {});
}

Program BuildTestProgram3Expected() {
  // Input:
  // .decl foo(bar: Bar)
  // .decl isBar(x: Bar)
  // .decl isPrincipal(x: Principal)
  // .decl isNumber(x: Number)
  // "PrinA" says foo(bar1) :- foo(bar2)
  datalog::RelationDeclaration foo(
      "foo", false,
      {
          datalog::Argument("bar",
                            datalog::ArgumentType::MakeCustomType("Bar")),
      });
  datalog::RelationDeclaration is_bar_decl(
      "isBar", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeCustomType("Bar")),
      });
  datalog::RelationDeclaration is_principal_decl(
      "isPrincipal", false,
      {datalog::Argument("x", datalog::ArgumentType::MakePrincipalType())});
  datalog::RelationDeclaration is_number_decl(
      "isNumber", false,
      {
          datalog::Argument("x", datalog::ArgumentType::MakeNumberType()),
      });
  SaysAssertion prina_universe_prina(
      Principal("\"PrinA\""),
      {Assertion(
          Fact({}, BaseFact(datalog::Predicate("isPrincipal", {"\"PrinA\""},
                                               datalog::kPositive))))});
  SaysAssertion assertion1(
      Principal("\"PrinA\""),
      {Assertion(ConditionalAssertion(
          Fact({}, BaseFact(datalog::Predicate("foo", {"bar1"},
                                               datalog::kPositive))),  // lhs
          {
              BaseFact(
                  datalog::Predicate("isBar", {"bar1"}, datalog::kPositive)),
              BaseFact(datalog::Predicate("foo", {"bar2"}, datalog::kPositive)),
          }  // rhs
          ))});
  return Program({foo, is_bar_decl, is_principal_decl, is_number_decl},
                 {assertion1, prina_universe_prina}, {});
}

struct ProgramEquivalenceTestData {
  Program input_program;
  Program expected_program;
};

class ProgramDebugStringEquivalenceTest
    : public testing::TestWithParam<ProgramEquivalenceTestData> {};

TEST_P(ProgramDebugStringEquivalenceTest,
       SimpleProgramDebugStringEquivalenceTest) {
  const auto& [input_program, expected_program] = GetParam();
  // declarations
  EXPECT_THAT(
      input_program.relation_declarations(),
      UnorderedElementsAreArray(expected_program.relation_declarations()));
  // says_assertions
  EXPECT_THAT(input_program.says_assertions(),
              UnorderedElementsAreArray(expected_program.says_assertions()));
  // queries
  EXPECT_THAT(input_program.queries(),
              UnorderedElementsAreArray(expected_program.queries()));
}

INSTANTIATE_TEST_SUITE_P(
    Something, ProgramDebugStringEquivalenceTest,
    testing::Values(
        ProgramEquivalenceTestData{InsertUniverseRelations(BuildTestProgram1()),
                                   BuildTestProgram1Expected()},
        ProgramEquivalenceTestData{InsertUniverseRelations(BuildTestProgram2()),
                                   BuildTestProgram2Expected()},
        ProgramEquivalenceTestData{InsertUniverseRelations(BuildTestProgram3()),
                                   BuildTestProgram3Expected()}));

}  // namespace raksha::ir::auth_logic
