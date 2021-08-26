#include "src/ir/access_path_selectors.h"

#include <string>
#include <vector>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/hash/hash_testing.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"
#include "src/common/testing/gtest.h"
#include "src/ir/field_selector.h"
#include "src/ir/selector.h"

namespace raksha::ir {

// Selector Access paths are easier to read and write in their string form, so
// this is very useful for creating readable test inputs. This could possibly
// be useful as a method in AccessPathSelectors itself, but, it's a bit
// simplified right now, as it only splits on '.', the field separator
// character. Also, I haven't found a use for it in non-test code yet.
static AccessPathSelectors MakeSelectorAccessPathFromString(
    std::string str) {
  std::vector<std::string> selector_strs =
      absl::StrSplit(std::move(str), '.', absl::SkipEmpty());

  // Start with an empty path and add all Selectors as parents.
  AccessPathSelectors result;
  for (auto iter = selector_strs.rbegin();
        iter != selector_strs.rend(); ++iter) {
    result = AccessPathSelectors(
        Selector(FieldSelector(std::move(*iter))), std::move(result));
  }

  return result;
}

class AccessPathEqualsTest :
   public ::testing::TestWithParam<::std::tuple<std::string, std::string>> {};

// This test ensures that, given a pair of access path strings, the access
// paths will be equal only if the original strings are equal. This allows us
// to have some confidence that equals works as expected.
TEST_P(AccessPathEqualsTest, AccessPathsForEqualStringsCompareEquals) {
  std::string access_path_str1;
  std::string access_path_str2;
  std::tie(access_path_str1, access_path_str2) = GetParam();

  const bool access_paths_strs_equal = access_path_str1 == access_path_str2;

  const absl::StatusOr<AccessPathSelectors> access_path1 =
      MakeSelectorAccessPathFromString(access_path_str1);
  ASSERT_TRUE(access_path1.ok());

  const absl::StatusOr<AccessPathSelectors> access_path2 =
      MakeSelectorAccessPathFromString(access_path_str2);
  ASSERT_TRUE(access_path2.ok());

  ASSERT_EQ(*access_path1 == *access_path2, access_paths_strs_equal);
}

// A selection of interesting and valid access_path_strs to use as inputs to
// various tests.
static const std::string access_path_strs[] = {
  "",
  ".comp1",
  ".comp2",
  ".comp1.comp2",
  ".comp1comp2",
  ".c",
  ".x.y.z",
  ".x.y.z.w",
  ".w.x.y.z"
};

// For the Equals test, test it on each pair of strings from access_path_strs.
INSTANTIATE_TEST_SUITE_P(
    AccessPathEqTestSuite,
    AccessPathEqualsTest,
    // Get the cartesian product of the list of input strs with itself.
    testing::Combine(
        testing::ValuesIn(access_path_strs),
        testing::ValuesIn(access_path_strs)));

class AccessPathTest : public ::testing::TestWithParam<std::string> {};

// A string with components separated by dots and an AccessPathSelectors are
// different representations of the same information. We should lose no
// information by moving from one to the other and then back and should be able
// to round trip between these representations.
//
// Perform this with a number of access paths.
TEST_P(AccessPathTest, CanRoundTripAccessPathString) {
  std::string original_access_path = GetParam();

  const absl::StatusOr<AccessPathSelectors> access_path =
      MakeSelectorAccessPathFromString(original_access_path);

  ASSERT_TRUE(access_path.ok());
  // Assert that the result of to_string is the same as the original AccessPathSelectors.
  ASSERT_EQ(access_path->ToString(), original_access_path);
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathTestsWithPaths,
    AccessPathTest,
    testing::ValuesIn(access_path_strs)
);

// Check that our Absl hash function works as expected for the
// AccessPathSelectors we provide.
TEST(AccessPathHashTest, SelectorAccessPathHashTest) {
  std::vector<AccessPathSelectors> access_paths_to_check;
  for (const std::string &access_path_str : access_path_strs) {
    absl::StatusOr<AccessPathSelectors> access_path =
        MakeSelectorAccessPathFromString(access_path_str);
    ASSERT_TRUE(access_path.ok());
    access_paths_to_check.push_back(std::move(*access_path));
  }

  EXPECT_TRUE(
      absl::VerifyTypeImplementsAbslHashCorrectly(access_paths_to_check));
}

static const std::tuple<std::string, std::string>
  access_path_selectors_proto_tostring_pairs[] {
    {"", ""},
    { "selectors: { field: \"foo\" }", ".foo" },
    { "selectors: [{ field: \"foo\" }, { field: \"bar\" }]", ".foo.bar" },
    { "selectors: [{ field: \"foo\" }, { field: \"bar\" }, { field: \"baz\" }]",
      ".foo.bar.baz" },
};

class AccessPathSelectorsFromProtoTest
 : public testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(AccessPathSelectorsFromProtoTest, AccessPathSelectorsFromProtoTest) {
  std::string access_path_textproto = std::get<0>(GetParam());
  std::string expected_to_string = std::get<1>(GetParam());

  arcs::AccessPathProto orig_access_path_proto;
  google::protobuf::TextFormat::ParseFromString(
      access_path_textproto, &orig_access_path_proto);

  // If this test were given an AccessPathProto with a root element, it would
  // fail because this class handles the selector piece only. Guard ourselves
  // from confusion due to bad inputs by checking that our input does not
  // have a root.
  EXPECT_FALSE(orig_access_path_proto.has_handle());
  EXPECT_FALSE(orig_access_path_proto.has_store_id());

  AccessPathSelectors access_path_selectors =
      AccessPathSelectors::CreateFromProto(orig_access_path_proto);
  EXPECT_EQ(access_path_selectors.ToString(), expected_to_string);

  arcs::AccessPathProto result_access_path_proto;
  access_path_selectors.FillSelectors(result_access_path_proto);
  EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
      result_access_path_proto, orig_access_path_proto));
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathSelectorsFromProtoTest, AccessPathSelectorsFromProtoTest,
    testing::ValuesIn(access_path_selectors_proto_tostring_pairs));

} // namespace raksha::ir
