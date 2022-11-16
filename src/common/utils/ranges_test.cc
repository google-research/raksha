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
#include "src/common/utils/ranges.h"

#include <unordered_set>
#include <vector>

#include "src/common/testing/gtest.h"

namespace raksha::utils::ranges {
namespace {

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(RangesTest, AllReturnsAllValues) {
  std::vector<int> int_vector = {1, 2, 6, 3, 3, 5};
  EXPECT_THAT(all(int_vector), ElementsAre(1, 2, 6, 3, 3, 5));

  std::unordered_set<int> int_set = {1, 2, 6, 3, 3, 5};
  EXPECT_THAT(all(int_set), UnorderedElementsAre(1, 2, 3, 5, 6));

  std::unordered_map<int, std::string> int_map = {
      {1, "one"}, {2, "two"}, {6, "six"}, {3, "three"}, {5, "five"}};
  EXPECT_THAT(all(int_map), UnorderedElementsAre(
                                Pair(1, "one"), Pair(2, "two"), Pair(6, "six"),
                                Pair(3, "three"), Pair(5, "five")));
}

TEST(RangesTest, KeysIteratesOverKeys) {
  std::unordered_map<int, std::string> int_map = {
      {1, "one"}, {2, "two"}, {6, "six"}, {3, "three"}, {5, "five"}};
  EXPECT_THAT(keys(int_map), UnorderedElementsAre(1, 2, 3, 5, 6));
}

TEST(RangesTest, ValuesIteratesOverValues) {
  std::unordered_map<int, std::string> int_map = {
      {1, "one"}, {2, "two"}, {6, "six"}, {3, "three"}, {5, "five"}};
  EXPECT_THAT(values(int_map),
              UnorderedElementsAre("one", "two", "three", "six", "five"));
}

}  // namespace

}  // namespace raksha::utils::ranges
