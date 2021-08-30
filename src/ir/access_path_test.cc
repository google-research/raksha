#include "src/ir/access_path.h"

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

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
  EXPECT_EQ(access_path.ToString(), expected_to_string);
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
  AccessPath access_path =
      spec_access_path.Instantiate(
          AccessPathRoot(
              HandleConnectionAccessPathRoot("recipe", "particle", "handle")));
  ASSERT_EQ(access_path.ToString(), "recipe.particle.handle.x");
  ASSERT_DEATH(
      access_path.Instantiate(
          AccessPathRoot(
              HandleConnectionAccessPathRoot(
                  "recipe2", "particle2", "handle2"))),
      "Attempt to instantiate an AccessPath that is already instantiated.");
}

static const std::tuple<std::string, std::string>
  access_path_proto_tostring_pairs[] {
    { "handle: { particle_spec: \"ps\", handle_connection: \"hc\" }", ""},
    { "handle: { particle_spec: \"ps\", handle_connection: \"hc\" } "
      "selectors: { field: \"foo\" }", ".foo" },
    { "handle: { particle_spec: \"ps\", handle_connection: \"hc\" } "
      "selectors: [{ field: \"foo\" }, { field: \"bar\" }]", ".foo.bar" },
    { "handle: { particle_spec: \"ps\", handle_connection: \"hc\" } "
      "selectors: [{ field: \"foo\" }, { field: \"bar\" }, { field: \"baz\" }]",
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

  AccessPathRoot root(
      HandleConnectionAccessPathRoot("recipe", "particle", "handle"));
  ASSERT_EQ(
      access_path.Instantiate(root).ToString(),
      "recipe.particle.handle" + expected_tostring_suffix);
  ASSERT_DEATH(
      access_path.ToString(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

INSTANTIATE_TEST_SUITE_P(AccessPathFromProtoTest, AccessPathFromProtoTest,
                         testing::ValuesIn(access_path_proto_tostring_pairs));

}  // namespace raksha::ir
