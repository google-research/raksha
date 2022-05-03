//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/map_iter.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/ast_construction.h"
#include "src/ir/auth_logic/ast_string.h"

namespace raksha::ir::auth_logic {

using ::testing::UnorderedElementsAre;

struct RelationDeclarationTestData {
  std::string input_text;
  std::string relation_name;
  bool is_attribute;
  std::vector<std::string> arguments;
};

class RelationDeclarationAstConstructionTest
    : public testing::TestWithParam<RelationDeclarationTestData> {};

TEST_P(RelationDeclarationAstConstructionTest,
       SimpleTestWithRelationDeclarationProgram) {
  const auto& [input_text, relation_name, is_attribute, arguments] = GetParam();
  Program prog_out = ParseProgram(input_text);
  ASSERT_EQ(prog_out.relation_declarations().size(), 1);
  const RelationDeclaration& relation_declaration =
      prog_out.relation_declarations().front();
  EXPECT_EQ(relation_declaration.relation_name(), relation_name);
  EXPECT_EQ(relation_declaration.is_attribute(), is_attribute);
  EXPECT_EQ(relation_declaration.arguments().size(), arguments.size());
  std::vector<std::string> argument_string = utils::MapIter<std::string>(
      relation_declaration.arguments(),
      [](Argument argument) { return ToString(argument); });
  EXPECT_THAT(argument_string, ::testing::UnorderedElementsAreArray(arguments));
}

INSTANTIATE_TEST_SUITE_P(
    RelationDeclarationAstConstructionTest,
    RelationDeclarationAstConstructionTest,
    testing::Values(
        RelationDeclarationTestData({R"(.decl someFact(x : CustomType))",
                                     R"(someFact)",
                                     false,
                                     {R"(x : CustomType)"}}),
        RelationDeclarationTestData(
            {R"(.decl someOtherFact(x : CustomType, y : Number))",
             R"(someOtherFact)",
             false,
             {R"(x : CustomType)", R"(y : Number)"}}),
        RelationDeclarationTestData({R"(.decl thirdFact(p : Principal).)",
                                     R"(thirdFact)",
                                     false,
                                     {R"(p : Principal)"}}),
        RelationDeclarationTestData(
            {R"(.decl attribute fourthFact(p : Principal).)",
             R"(fourthFact)",
             true,
             {R"(p : Principal)"}})

            ));

}  // namespace raksha::ir::auth_logic
