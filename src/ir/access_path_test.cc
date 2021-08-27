#include "src/common/testing/gtest.h"
#include "src/ir/access_path.h"

namespace raksha::ir {

static Selector MakeFieldSelector(std::string field_name) {
  return Selector(FieldSelector(field_name));
}

static const std::tuple<AccessPath, std::string>
    access_path_and_expected_tostring_pairs[] = {
    {AccessPath(AccessPathRoot(ConcreteAccessPathRoot("a.b")),
             AccessPathSelectors(
                 MakeFieldSelector("c"),
                 AccessPathSelectors(
                     MakeFieldSelector("d"), AccessPathSelectors()))),
     "a.b.c.d" },
    { AccessPath(
        AccessPathRoot(ConcreteAccessPathRoot("foo")), AccessPathSelectors()),
      "foo" },
    { AccessPath(
        AccessPathRoot(ConcreteAccessPathRoot("foo.bar")),
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

TEST(InstantiateAccessPathTest, InstantiateAccessPathTest) {
  AccessPath spec_access_path =
      AccessPath::CreateSpecAccessPath(
          AccessPathSelectors(Selector(FieldSelector("x"))));
  AccessPath access_path =
      AccessPath::Instantiate(ConcreteAccessPathRoot("concrete"),
                              spec_access_path);
  ASSERT_EQ(access_path.ToString(), "concrete.x");
  ASSERT_DEATH(
      AccessPath::Instantiate(ConcreteAccessPathRoot("uhoh"), access_path),
      "Attempt to instantiate an AccessPath that is already instantiated.");
}


}  // namespace raksha::ir
