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

#include "src/common/utils/move_append.h"

#include <vector>

#include "src/common/testing/gtest.h"

namespace raksha::utils {

TEST(MoveAppendTestSuite, SimpleMoveAppendTest) {
  std::vector test_vec1({1, 2, 3, 4});
  std::vector test_vec2({5, 6, 7, 8});
  MoveAppend(test_vec1, std::move(test_vec2));
  EXPECT_EQ(test_vec1, std::vector({1, 2, 3, 4, 5, 6, 7, 8}));
}

}  // namespace raksha::utils
