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
//-----------------------------------------------------------------------------
#include "src/utils/ref_counter.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"

namespace raksha {
namespace {
template <typename T>
class RefCounterTest : public ::testing::Test {};

using RefCounterTypes = testing::Types<RefCounter<false>, RefCounter<true>>;
TYPED_TEST_SUITE(RefCounterTest, RefCounterTypes);

TYPED_TEST(RefCounterTest, Initialization) {
  TypeParam zero_count(0);
  EXPECT_EQ(zero_count, 0);
  TypeParam five_count(5);
  EXPECT_EQ(five_count, 5);
}

TYPED_TEST(RefCounterTest, FetchAddIncrementsAndReturnsOldValue) {
  TypeParam count(5);
  EXPECT_EQ(count.FetchAdd(2), 5);
  EXPECT_EQ(count, 7);
  EXPECT_EQ(count.FetchAdd(1), 7);
  EXPECT_EQ(count, 8);
}

TYPED_TEST(RefCounterTest, FetchSubDecrementsAndReturnsOldValue) {
  TypeParam count(5);
  EXPECT_EQ(count.FetchSub(2), 5);
  EXPECT_EQ(count, 3);
  EXPECT_EQ(count.FetchSub(1), 3);
  EXPECT_EQ(count, 2);
}

TYPED_TEST(RefCounterTest, FetchSubDecrementToLessThanZeroCausesFailure) {
  EXPECT_DEATH(
      {
        TypeParam count(2);
        count.FetchSub(3);
      },
      "FetchSub can decrement below zero.");
}

}  // namespace

}  // namespace raksha
