#include "src/common/testing/gtest.h"
#include "src/ir/access_path.h"

namespace raksha::ir {

static Selector MakeFieldSelector(std::string field_name) {
  return Selector(FieldSelector(field_name));
}

static const std::tuple<AccessPath, std::string>
    access_path_and_expected_tostring_pairs[] = {
    {AccessPath("a.b",
             AccessPathSelectors(
                 MakeFieldSelector("c"),
                 AccessPathSelectors(
                     MakeFieldSelector("d"), AccessPathSelectors()))),
     "a.b.c.d" },
    { AccessPath("foo", AccessPathSelectors()), "foo" },
    { AccessPath(
        "foo.bar",
        AccessPathSelectors(
            MakeFieldSelector("baz"), AccessPathSelectors())), "foo.bar.baz"}
};

class AccessPathToStringTest :
 public testing::TestWithParam<std::tuple<AccessPath, std::string>> {};

TEST_P(AccessPathToStringTest, AccessPathToStringTest) {
  const AccessPath &access_path = std::get<0>(GetParam());
  const std::string &expected_to_string = std::get<1>(GetParam());
  EXPECT_EQ(access_path.ToString(), expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathToStringTest, AccessPathToStringTest,
    testing::ValuesIn(access_path_and_expected_tostring_pairs));


}  // namespace raksha::ir
