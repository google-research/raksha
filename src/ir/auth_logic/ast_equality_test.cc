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
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(p1 == p2);
}

TEST(AstEqualityTestSuite, Test2) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("food", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test3) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"baz", "bar"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test4) {
  datalog::Predicate p1("foo", {"barrrrr", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test5) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "bas"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test6) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz"}, datalog::kNegated);
  EXPECT_TRUE(!(p1 == p2));
}

TEST(AstEqualityTestSuite, Test7) {
  datalog::Predicate p1("foo", {"bar", "baz"}, datalog::kPositive);
  datalog::Predicate p2("foo", {"bar", "baz", "beef"}, datalog::kPositive);
  EXPECT_TRUE(!(p1 == p2));
}

}  // namespace raksha::ir::auth_logic
