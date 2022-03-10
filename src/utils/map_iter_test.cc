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

#include "absl/container/flat_hash_map.h"
#include "src/common/testing/gtest.h"

namespace raksha::utils {

TEST(MapIterTestSuite, SimpleMapIterTest) {
  std::vector test_vec = MapIterVal<std::vector<int>>(
      std::vector({1, 5, 3, 9, 5}), [](const int& x) { return x + 3; });
  EXPECT_EQ(test_vec, std::vector({4, 8, 6, 12, 8}));
}

TEST(MapIterTestSuite, MapIterToMapTest) {
  absl::flat_hash_map<int, int> test_map =
      MapIterVal<absl::flat_hash_map<int, int>>(
          std::vector({1, 5, 3, 9, 5}),
          [](const int& x) { return std::make_pair(x, x); },
          CollisionPolicy::kIgnore);
  EXPECT_EQ(test_map,
            (absl::flat_hash_map<int, int>({{1, 1}, {5, 5}, {3, 3}, {9, 9}})));
}

TEST(MapIterTestSuite, MapIterToSetTest) {
  absl::flat_hash_set<int> test_set = MapIterVal<absl::flat_hash_set<int>>(
      std::vector({1, 5, 3, 9, 5}), [](const int& x) { return x + 3; },
      CollisionPolicy::kIgnore);
  EXPECT_EQ(test_set, (absl::flat_hash_set<int>({4, 8, 6, 12})));
}

TEST(MapIterTestSuite, MapIterToMapCollisionDeath) {
  EXPECT_DEATH(
      {
        (MapIterVal<absl::flat_hash_map<int, int>>(
            (std::vector({1, 5, 3, 9, 5})),
            ([](const int& x) { return std::make_pair(x, x); })));
      },
      "Found collision when mapping a container.");
}

TEST(MapIterTestSuite, MapIterToSetCollisionDeath) {
  EXPECT_DEATH(
      {
        (MapIterVal<absl::flat_hash_set<int>>(
            (std::vector({1, 5, 3, 9, 5})),
            ([](const int& x) { return x + 3; })));
      },
      "Found collision when mapping a container.");
}

TEST(MapIterTestSuite, MapIterToMapTestNoCollisions) {
  absl::flat_hash_map<int, int> test_map =
      MapIterVal<absl::flat_hash_map<int, int>>(
          std::vector({1, 5, 3, 9}),
          [](const int& x) { return std::make_pair(x, x); });
  EXPECT_EQ(test_map,
            (absl::flat_hash_map<int, int>({{1, 1}, {5, 5}, {3, 3}, {9, 9}})));
}

TEST(MapIterTestSuite, MapIterToSetTestNoCollisions) {
  absl::flat_hash_set<int> test_set = MapIterVal<absl::flat_hash_set<int>>(
      std::vector({1, 5, 3, 9}), [](const int& x) { return x + 3; });
  EXPECT_EQ(test_set, (absl::flat_hash_set<int>({4, 8, 6, 12})));
}

TEST(MapIterRefTestSuite, SimpleMapIterRefTest) {
  std::vector test_vec = MapIterRef<std::vector<int>>(
      std::vector({1, 5, 3, 9, 5}), [](const int& x) { return x + 3; });
  EXPECT_EQ(test_vec, std::vector({4, 8, 6, 12, 8}));
}

TEST(MapIterRefTestSuite, MapIterToMapTest) {
  absl::flat_hash_map<int, int> test_map =
      MapIterRef<absl::flat_hash_map<int, int>>(
          std::vector({1, 5, 3, 9, 5}),
          [](const int& x) { return std::make_pair(x, x); },
          CollisionPolicy::kIgnore);
  EXPECT_EQ(test_map,
            (absl::flat_hash_map<int, int>({{1, 1}, {5, 5}, {3, 3}, {9, 9}})));
}

TEST(MapIterRefTestSuite, MapIterToSetTest) {
  absl::flat_hash_set<int> test_set = MapIterRef<absl::flat_hash_set<int>>(
      std::vector({1, 5, 3, 9, 5}), [](const int& x) { return x + 3; },
      CollisionPolicy::kIgnore);
  EXPECT_EQ(test_set, (absl::flat_hash_set<int>({4, 8, 6, 12})));
}

TEST(MapIterRefTestSuite, MapIterToMapCollisionDeath) {
  EXPECT_DEATH(
      {
        (MapIterRef<absl::flat_hash_map<int, int>>(
            (std::vector({1, 5, 3, 9, 5})),
            ([](const int& x) { return std::make_pair(x, x); })));
      },
      "Found collision when mapping a container.");
}

TEST(MapIterRefTestSuite, MapIterToSetCollisionDeath) {
  EXPECT_DEATH(
      {
        (MapIterRef<absl::flat_hash_set<int>>(
            (std::vector({1, 5, 3, 9, 5})),
            ([](const int& x) { return x + 3; })));
      },
      "Found collision when mapping a container.");
}

TEST(MapIterRefTestSuite, MapIterToMapTestNoCollisions) {
  absl::flat_hash_map<int, int> test_map =
      MapIterRef<absl::flat_hash_map<int, int>>(
          std::vector({1, 5, 3, 9}),
          [](const int& x) { return std::make_pair(x, x); });
  EXPECT_EQ(test_map,
            (absl::flat_hash_map<int, int>({{1, 1}, {5, 5}, {3, 3}, {9, 9}})));
}

TEST(MapIterRefTestSuite, MapIterToSetTestNoCollisions) {
  absl::flat_hash_set<int> test_set = MapIterRef<absl::flat_hash_set<int>>(
      std::vector({1, 5, 3, 9}), [](const int& x) { return x + 3; });
  EXPECT_EQ(test_set, (absl::flat_hash_set<int>({4, 8, 6, 12})));
}

}  // namespace raksha::utils
