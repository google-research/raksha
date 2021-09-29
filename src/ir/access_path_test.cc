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

#include "src/ir/access_path.h"

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"
#include "src/ir/map_instantiator.h"
#include "src/ir/noop_instantiator.h"

namespace raksha::ir {

static Selector MakeFieldSelector(std::string field_name) {
  return Selector(FieldSelector(field_name));
}

static const std::tuple<AccessPath, std::string>
    access_path_and_expected_tostring_pairs[] = {
    {AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot("a", "b", "c")),
             AccessPathSelectors(
                 MakeFieldSelector("d"),
                 AccessPathSelectors(
                     MakeFieldSelector("e"), AccessPathSelectors()))),
     "a.b.c.d.e" },
    { AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        AccessPathSelectors()),
      "foo.bar.baz" },
    { AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        AccessPathSelectors(
            MakeFieldSelector("field"), AccessPathSelectors())),
      "foo.bar.baz.field"}
};

class AccessPathToStringTest :
 public testing::TestWithParam<std::tuple<AccessPath, std::string>> {};

TEST_P(AccessPathToStringTest, AccessPathToStringTest) {
  const AccessPath &access_path = std::get<0>(GetParam());
  const std::string &expected_to_string = std::get<1>(GetParam());
  EXPECT_EQ(
      access_path.ToString(NoopInstantiator::Get()), expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathToStringTest, AccessPathToStringTest,
    testing::ValuesIn(access_path_and_expected_tostring_pairs));

TEST(InstantiateAccessPathTest, InstantiateAccessPathTest) {
  AccessPath spec_access_path(
      AccessPathRoot(
          HandleConnectionSpecAccessPathRoot(
              "particle_spec", "handle_connection")),
      AccessPathSelectors(Selector(FieldSelector("x"))));

  MapInstantiator map_instantiator(
      absl::flat_hash_map<AccessPathRoot, AccessPathRoot>{
        { AccessPathRoot(HandleConnectionSpecAccessPathRoot(
            "particle_spec", "handle_connection")),
        AccessPathRoot(HandleConnectionAccessPathRoot(
            "recipe", "particle", "handle")) } });
  ASSERT_EQ(
      spec_access_path.ToString(map_instantiator), "recipe.particle.handle.x");
}

class AccessPathEqualsTest :
 public testing::TestWithParam<
  std::tuple<
    std::tuple<ir::AccessPathRoot, ir::AccessPathSelectors>,
    std::tuple<ir::AccessPathRoot, ir::AccessPathSelectors>>> {};

TEST_P(AccessPathEqualsTest, AccessPathEqualsTest) {
  const std::tuple<ir::AccessPathRoot, ir::AccessPathSelectors>
      access_path_args1 = std::get<0>(GetParam());
  const std::tuple<ir::AccessPathRoot, ir::AccessPathSelectors>
    access_path_args2 = std::get<1>(GetParam());

  AccessPath access_path1(
      std::get<0>(access_path_args1), std::get<1>(access_path_args1));
  AccessPath access_path2(
    std::get<0>(access_path_args2), std::get<1>(access_path_args2));

  EXPECT_EQ(
      access_path1 == access_path2, access_path_args1 == access_path_args2);
}

// Note: be careful adding elements to sample_roots and sample_selectors. We
// create an access path for each combination of elements in
// (sample_roots, sample_selectors) and then compare each of those access
// paths against each other. That means that we generate a number of tests
// equal to the square of the size of sample_roots * sample_selectors.
static AccessPathRoot sample_roots[] = {
    ir::AccessPathRoot(ir::HandleConnectionSpecAccessPathRoot(
        "spec1", "handle_spec1")),
    ir::AccessPathRoot(ir::HandleConnectionAccessPathRoot(
        "recipe", "particle", "handle")),
    ir::AccessPathRoot(ir::HandleAccessPathRoot("recipe", "handle"))
};

static AccessPathSelectors sample_selectors[] = {
    ir::AccessPathSelectors(),
    ir::AccessPathSelectors(ir::Selector(ir::FieldSelector("field1"))),
    ir::AccessPathSelectors(
        ir::Selector(ir::FieldSelector("x")),
        ir::AccessPathSelectors(ir::Selector(ir::FieldSelector("y"))))
};

INSTANTIATE_TEST_SUITE_P(
    AccessPathEqualsTest, AccessPathEqualsTest,
    testing::Combine(
        testing::Combine(
            testing::ValuesIn(sample_roots),
            testing::ValuesIn(sample_selectors)),
        testing::Combine(
            testing::ValuesIn(sample_roots),
            testing::ValuesIn(sample_selectors))));

TEST(AccessPathHashTest, AccessPathHashTest) {
  std::vector<AccessPath> test_access_paths;
  for (const AccessPathRoot &access_path_root : sample_roots) {
    for (const AccessPathSelectors &access_path_selectors : sample_selectors) {
      test_access_paths.push_back(
          AccessPath(access_path_root, access_path_selectors));
    }
  }
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(test_access_paths));
}

}  // namespace raksha::ir
