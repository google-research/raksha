//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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

#include "program.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::datalog {

TEST(IsNumericOperator, BasicTest) {
  EXPECT_EQ(Predicate("<", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(Predicate("<", {}, Sign::kNegated).IsNumericOperator(), true);
  EXPECT_EQ(Predicate("<", {"arg1"}, Sign::kPositive).IsNumericOperator(),
            true);
  EXPECT_EQ(Predicate(">", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(Predicate("=", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(Predicate("!=", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(Predicate("<=", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(Predicate(">=", {}, Sign::kPositive).IsNumericOperator(), true);
  EXPECT_EQ(
      Predicate("something_else", {}, Sign::kPositive).IsNumericOperator(),
      false);
  EXPECT_EQ(Predicate("something_else", {}, Sign::kNegated).IsNumericOperator(),
            false);
  EXPECT_EQ(Predicate("another_thing", {}, Sign::kNegated).IsNumericOperator(),
            false);
  EXPECT_EQ(
      Predicate("another_thing", {"arg"}, Sign::kNegated).IsNumericOperator(),
      false);
}

};  // namespace raksha::ir::datalog