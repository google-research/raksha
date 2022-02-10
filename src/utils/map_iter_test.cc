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
#include "src/utils/map_iter.h"

#include "src/common/testing/gtest.h"

namespace raksha::utils {
TEST(MapIterTestSuite, SimpleMapIterTest) {
  std::vector test_vec = MapIter<int, int>(std::vector({1, 5, 3, 9, 5}),
                                           [](const int& x) { return x + 3; });
  EXPECT_EQ(test_vec, std::vector({4, 8, 6, 12, 8}));
}
}  // namespace raksha::utils
