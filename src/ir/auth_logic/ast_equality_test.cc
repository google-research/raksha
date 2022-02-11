//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/common/testing/gtest.h"
#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

TEST(AstEqualityTestSuite, Test1) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("foo", {"bar", "baz"}, kPositive);
  EXPECT_TRUE(p1 == p2);
}

TEST(AstEqualityTestSuite, Test2) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("food", {"bar", "baz"}, kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test3) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("foo", {"baz", "bar"}, kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test4) {
  Predicate p1("foo", {"barrrrr", "baz"}, kPositive);
  Predicate p2("foo", {"bar", "baz"}, kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test5) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("foo", {"bar", "bas"}, kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test6) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("foo", {"bar", "baz"}, kNegated);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test7) {
  Predicate p1("foo", {"bar", "baz"}, kPositive);
  Predicate p2("foo", {"bar", "baz", "beef"}, kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

}  // namespace raksha::ir::auth_logic
