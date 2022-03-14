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
#include "src/common/utils/ref_counter.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"

namespace raksha {
namespace {

class RefCounterTest : public ::testing::Test {};

TEST_F(RefCounterTest, Initialization) {
  RefCounter zero_count(0);
  EXPECT_EQ(zero_count, 0);
  RefCounter five_count(5);
  EXPECT_EQ(five_count, 5);
}

TEST_F(RefCounterTest, FetchAddIncrementsAndReturnsOldValue) {
  RefCounter count(5);
  EXPECT_EQ(count.FetchAdd(2), 5);
  EXPECT_EQ(count, 7);
  EXPECT_EQ(count.FetchAdd(1), 7);
  EXPECT_EQ(count, 8);
}

TEST_F(RefCounterTest, FetchSubDecrementsAndReturnsOldValue) {
  RefCounter count(5);
  EXPECT_EQ(count.FetchSub(2), 5);
  EXPECT_EQ(count, 3);
  EXPECT_EQ(count.FetchSub(1), 3);
  EXPECT_EQ(count, 2);
}

TEST_F(RefCounterTest, FetchSubDecrementToLessThanZeroCausesFailure) {
  EXPECT_DEATH(
      {
        RefCounter count(2);
        count.FetchSub(3);
      },
      "FetchSub can decrement below zero.");
}

}  // namespace

}  // namespace raksha
