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
#include "src/common/utils/map_iter.h"

#include <deque>
#include <vector>

#include "src/common/testing/gtest.h"

namespace raksha::utils {

TEST(MapIterTestSuite, SimpleMapIterTest) {
  std::vector test_vec = MapIter<int>(std::vector({1, 5, 3, 9, 5}),
                                      [](const int& x) { return x + 3; });
  EXPECT_THAT(test_vec, testing::ElementsAre(4, 8, 6, 12, 8));
}

TEST(MapIterTestSuite, SimpleMapIterOnDequeTest) {
  std::vector test_vec = MapIter<int>(std::deque({1, 5, 3, 9, 5}),
                                      [](const int& x) { return x + 4; });
  EXPECT_THAT(test_vec, testing::ElementsAre(5, 9, 7, 13, 9));
}

}  // namespace raksha::utils
