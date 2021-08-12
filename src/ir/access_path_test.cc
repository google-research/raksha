#include "src/ir/access_path.h"

#include "src/common/testing/gtest.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"

#include <string>

namespace raksha::ir {

// Access paths are easier to read and write in their string form, so this is
// very useful for creating readable test inputs.
static absl::StatusOr<AccessPath> make_access_path_from_string(
    const absl::string_view str) {
  const std::vector<absl::string_view> components =
      absl::StrSplit(str, '.');

  // Instantiate the AccessPath with the last element initially.
  auto components_rev_iter = components.rbegin();
  absl::StatusOr<AccessPath> access_path =
      AccessPath::Create(*components_rev_iter);
  if (!access_path.ok()) {
    return access_path.status();
  }
  ++components_rev_iter;

  // Add all others as parents.
  for (; components_rev_iter < components.rend(); ++components_rev_iter) {
    access_path = AccessPath::CreateParent(
        *components_rev_iter,
        *access_path);
    if (!access_path.ok()) {
      return access_path.status();
    }
  }
  return access_path;
}

class AccessPathEqualsTest :
   public ::testing::TestWithParam<::std::tuple<std::string, std::string>> {};

TEST_P(AccessPathEqualsTest, AccessPathsForEqualStringsCompareEquals) {
  std::string access_path_str1;
  std::string access_path_str2;
  std::tie(access_path_str1, access_path_str2) = GetParam();

  const bool access_paths_strs_equal = access_path_str1 == access_path_str2;

  const absl::StatusOr<AccessPath> access_path1 =
      make_access_path_from_string(access_path_str1);
  ASSERT_TRUE(access_path1.ok());

  const absl::StatusOr<AccessPath> access_path2 =
      make_access_path_from_string(access_path_str2);
  ASSERT_TRUE(access_path2.ok());

  ASSERT_EQ(*access_path1 == *access_path2, access_paths_strs_equal);
}

// A selection of interesting and valid access_path_strs to use as inputs to
// various tests.
static const std::string access_path_strs[] = {
  "comp1",
  "comp2",
  "comp1.comp2",
  "comp1comp2",
  "c",
  "x.y.z",
  "x.y.z.w",
  "w.x.y.z"
};

INSTANTIATE_TEST_SUITE_P(
    AccessPathEqTestSuite,
    AccessPathEqualsTest,
    // Get the cartesian product of the list of input strs with itself.
    testing::Combine(
        testing::ValuesIn(access_path_strs),
        testing::ValuesIn(access_path_strs)));

class AccessPathTest : public ::testing::TestWithParam<std::string> {};

// A string with components separated by dots and an AccessPath are different
// representations of the same information. We should lose no information by
// moving from one to the other and then back and should be able to round
// trip between these representations.
//
// Perform this with a number of access paths.
TEST_P(AccessPathTest, CanRoundTripAccessPathString) {
  const std::string original_access_path = GetParam();

  const absl::StatusOr<AccessPath> access_path =
      make_access_path_from_string(original_access_path);

  ASSERT_TRUE(access_path.ok());
  // Assert that the result of to_string is the same as the original AccessPath.
  ASSERT_EQ(access_path->ToString(), original_access_path);
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathTestsWithPaths,
    AccessPathTest,
    testing::ValuesIn(access_path_strs)
);

TEST(AccessPathFailTest, EmptyAccessPathLeafComponent) {
  absl::StatusOr<AccessPath> empty_result = AccessPath::Create("");
  ASSERT_FALSE(empty_result.ok());
  ASSERT_EQ(empty_result.status().code(), absl::StatusCode::kInvalidArgument);
  ASSERT_EQ(
      empty_result.status().message(),
      "Empty component name not allowed in AccessPath.");
}

TEST(AccessPathFailTest, EmptyAccessPathParentComponent) {
  absl::StatusOr<AccessPath> leaf = AccessPath::Create("leaf");
  ASSERT_TRUE(leaf.ok());
  absl::StatusOr<AccessPath> empty_parent_result =
      AccessPath::CreateParent("", *leaf);
  ASSERT_FALSE(empty_parent_result.ok());
  ASSERT_EQ(
      empty_parent_result.status().code(),
      absl::StatusCode::kInvalidArgument);
  ASSERT_EQ(
      empty_parent_result.status().message(),
      "Empty component name not allowed in AccessPath.");
}

TEST(AccessPathFailTest, CompoundAccessPathLeafComponent) {
   absl::StatusOr<AccessPath> compound_result =
       AccessPath::Create("compound.leaf");
  ASSERT_FALSE(compound_result.ok());
  ASSERT_EQ(
      compound_result.status().code(),
      absl::StatusCode::kInvalidArgument);
  const std::string expected_message =
      "Access path component: compound.leaf contains a '.' and thus is "
      "compound; only simple component names are allowed.";
  ASSERT_EQ(
      compound_result.status().message(),
      expected_message);
}

TEST(AccessPathFailTest, CompoundAccessPathParentComponent) {
  absl::StatusOr<AccessPath> leaf = AccessPath::Create("leaf");
  ASSERT_TRUE(leaf.ok());
  absl::StatusOr<AccessPath> compound_parent_result =
      AccessPath::CreateParent("compound.parent", *leaf);
  ASSERT_FALSE(compound_parent_result.ok());
  ASSERT_EQ(
      compound_parent_result.status().code(),
      absl::StatusCode::kInvalidArgument);
  ASSERT_EQ(
      compound_parent_result.status().message(),
      "Access path component: compound.parent contains a '.' and thus is "
      "compound; only simple component names are allowed.");
}

} // namespace raksha::ir
