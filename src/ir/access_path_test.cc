#include "src/ir/access_path.h"

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

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
      spec_access_path.Instantiate(ConcreteAccessPathRoot("concrete"));
  ASSERT_EQ(access_path.ToString(), "concrete.x");
  ASSERT_DEATH(
      access_path.Instantiate(ConcreteAccessPathRoot("uhoh")),
      "Attempt to instantiate an AccessPath that is already instantiated.");
}

static const std::tuple<std::string, std::string>
  access_path_proto_tostring_pairs[] {
    {"", ""},
    { "selectors: { field: \"foo\" }", ".foo" },
    { "selectors: [{ field: \"foo\" }, { field: \"bar\" }]", ".foo.bar" },
    { "selectors: [{ field: \"foo\" }, { field: \"bar\" }, { field: \"baz\" }]",
      ".foo.bar.baz" },
};

class AccessPathFromProtoTest
 : public testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(AccessPathFromProtoTest, AccessPathFromProtoTest) {
  std::string textproto;
  std::string expected_tostring_suffix;
  std::tie(textproto, expected_tostring_suffix) = GetParam();

  arcs::AccessPathProto access_path_proto;
  google::protobuf::TextFormat::ParseFromString(textproto, &access_path_proto);

  AccessPath access_path = AccessPath::CreateFromProto(access_path_proto);

  ConcreteAccessPathRoot root("root");
  ASSERT_EQ(
      access_path.Instantiate(root).ToString(),
      "root" + expected_tostring_suffix);
  ASSERT_DEATH(
      access_path.ToString(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

INSTANTIATE_TEST_SUITE_P(AccessPathFromProtoTest, AccessPathFromProtoTest,
                         testing::ValuesIn(access_path_proto_tostring_pairs));

}  // namespace raksha::ir
