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

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::datalog {

TEST(Predicate, HashTest) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      Predicate("", {}, Sign::kNegated),
      Predicate("", {}, Sign::kPositive),
      Predicate("empty_pred", {}, Sign::kPositive),
      Predicate("empty_pred", {}, Sign::kNegated),
      Predicate("some_pred", {"a1"}, Sign::kNegated),
      Predicate("some_pred", {"a1"}, Sign::kPositive),
      Predicate("some_other_pred", {"a1", "a2", "a3"}, Sign::kPositive),
      Predicate("some_other_pred", {"a1", "a2", "a3"}, Sign::kNegated),
  }));
}

TEST(ArgumentType, HashTest) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      ArgumentType::MakePrincipalType(),
      ArgumentType::MakeNumberType(),
      ArgumentType::MakeCustomType("something"),
      ArgumentType::MakeCustomType("something_else"),
  }));
}

TEST(Argument, HashTest) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(
      {Argument("somePrin", ArgumentType::MakePrincipalType()),
       Argument("59000", ArgumentType::MakeNumberType()),
       Argument("A", ArgumentType::MakeCustomType("TypeA")),
       Argument("B", ArgumentType::MakeCustomType("TypeB")),
       Argument("", ArgumentType::MakeCustomType("")),
       Argument("", ArgumentType::MakeNumberType()),
       Argument("", ArgumentType::MakePrincipalType())}));
}

TEST(RelationDeclaration, HashTest) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
      RelationDeclaration("", false, {}),
      RelationDeclaration("", true, {}),
      RelationDeclaration("some_relation", false, {}),
      RelationDeclaration("some_relation", true, {}),
      RelationDeclaration(
          "some_other_relation", true,
          {Argument("somePrin", ArgumentType::MakePrincipalType()),
           Argument("SomeNumber", ArgumentType::MakeNumberType())}),
      RelationDeclaration(
          "some_other_relation", false,
          {Argument("someCustom", ArgumentType::MakeCustomType("otherType"))}),
  }));
}

}  // namespace raksha::ir::datalog
