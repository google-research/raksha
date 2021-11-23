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
//-----------------------------------------------------------------------------
#include "src/utils/ranges.h"

#include <vector>

#include "src/common/testing/gtest.h"

namespace raksha::utils {

TEST(MakeRangeTest, FieldsAreInitializedCorrectly){
  std::vector<int> values = {0, 1, 2, 3, 4, 5};
  auto all = make_range(values.begin(), values.end());
  EXPECT_EQ(all.begin(), values.begin());
  EXPECT_EQ(all.end(), values.end());

  auto small_range = make_range(values.begin() + 1, values.begin() + 3);
  EXPECT_EQ(small_range.begin(), values.begin() + 1);
  EXPECT_EQ(small_range.end(), values.begin() + 3);
}

TEST(MakeRangeTest, EmptynessIsDetected) {
  std::vector<int> values = {0, 1, 2, 3, 4, 5};
  EXPECT_FALSE(make_range(values.begin(), values.end()).empty());
  EXPECT_TRUE(make_range(values.begin(), values.begin()).empty());
  EXPECT_TRUE(make_range(values.end(), values.end()).empty());
}

TEST(MakeRangeTest, AllowsIteration) {
  std::vector<int> values = {0, 1, 2, 3, 4, 5};
  EXPECT_THAT(make_range(values.begin(), values.end()),
              ::testing::ElementsAre(0, 1, 2, 3, 4, 5));
  EXPECT_THAT(make_range(values.begin(), values.begin() + 2),
              ::testing::ElementsAre(0, 1));
  EXPECT_THAT(make_range(values.begin() + 2, values.begin() + 4),
              ::testing::ElementsAre(2, 3));
}

}  // namespace raksha::utils
