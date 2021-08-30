#include "src/ir/tag_check.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

class TagCheckToStringWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::tuple<std::string, absl::ParsedFormat<'s'>>, AccessPathRoot>>
      {};

TEST_P(TagCheckToStringWithRootTest, TagCheckToStringWithRootTest) {
  const std::tuple<std::string, absl::ParsedFormat<'s'>>
      &textproto_format_string_pair = std::get<0>(GetParam());
  const std::string &check_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::ParsedFormat<'s'> expected_tostring_format_string =
    std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<1>(GetParam());

  const std::string &expected_tostring = absl::StrFormat(
      expected_tostring_format_string, root.ToString());
  arcs::CheckProto check_proto;
  google::protobuf::TextFormat::ParseFromString(check_textproto, &check_proto);
  TagCheck unrooted_tag_check = TagCheck::CreateFromProto(check_proto);
  TagCheck tag_check = unrooted_tag_check.Instantiate(root);
  // Expect the version with the concrete root to match the expected_tostring
  // when ToString is called upon it.
  ASSERT_EQ(tag_check.ToString(), expected_tostring);
  // However, the version with the spec root should fail when ToString is
  // called.
  EXPECT_DEATH(
      unrooted_tag_check.ToString(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

// Sample roots we can use to instantiate TagChecks.
static AccessPathRoot instantiated_roots[] = {
    AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe", "particle", "handle")),
    AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe2", "particle2", "handle2")),
    AccessPathRoot(HandleConnectionAccessPathRoot("r2", "p2", "h2")),
    AccessPathRoot(HandleConnectionAccessPathRoot("r2", "particle", "handle"))
};

// Pairs of textprotos and format strings. The format strings will become the
// expected ToString output when the root string is substituted for the %s.
// This allows us to test the result of combining each of the
// TagChecks derived from the textprotos with each of the root strings.
static std::tuple<std::string, absl::ParsedFormat<'s'>>
    textproto_to_expected_format_string[] = {
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } "
      "selectors: { field: \"field1\" } }, "
      "predicate: { label: { semantic_tag: \"tag\"} }",
      absl::ParsedFormat<'s'>("checkHasTag(\"%s.field1\", \"tag\").\n") },
    { "access_path: {"
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } }, "
      "predicate: { label: { semantic_tag: \"tag2\"} }",
      absl::ParsedFormat<'s'>("checkHasTag(\"%s\", \"tag2\").\n") },
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" }, "
      "selectors: [{ field: \"x\" }, { field: \"y\" }] }, "
      "predicate: { label: { semantic_tag: \"user_selection\"} }",
      absl::ParsedFormat<'s'>(
          "checkHasTag(\"%s.x.y\", \"user_selection\")" ".\n") }
};

INSTANTIATE_TEST_SUITE_P(
    TagCheckToStringWithRootTest, TagCheckToStringWithRootTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(instantiated_roots)));

}  // namespace raksha::ir
