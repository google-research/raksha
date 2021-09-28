#include "src/ir/tag_check.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

class TagCheckToDatalogWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::tuple<std::string, absl::ParsedFormat<'s', 's'>>, AccessPathRoot>>
      {};

TEST_P(TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest) {
  const std::tuple<std::string, absl::ParsedFormat<'s', 's'>>
      &textproto_format_string_pair = std::get<0>(GetParam());
  const std::string &check_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::ParsedFormat<'s', 's'> expected_todatalog_format_string =
    std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<1>(GetParam());
  std::string root_string = root.ToString();
  const std::string &expected_todatalog = absl::StrFormat(
      expected_todatalog_format_string, root_string, root_string);
  arcs::CheckProto check_proto;
  google::protobuf::TextFormat::ParseFromString(check_textproto, &check_proto);
  TagCheck unrooted_tag_check = TagCheck::CreateFromProto(check_proto);
  TagCheck tag_check = unrooted_tag_check.Instantiate(root);
  // Expect the version with the concrete root to match the expected_todatalog
  // when ToDatalog is called upon it.
  ASSERT_EQ(tag_check.ToDatalog(), expected_todatalog);
  // However, the version with the spec root should fail when ToDatalog is
  // called.
  EXPECT_DEATH(
      unrooted_tag_check.ToDatalog(),
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
// expected ToDatalog output when the root string is substituted for the %s.
// This allows us to test the result of combining each of the
// TagChecks derived from the textprotos with each of the root strings.
static std::tuple<std::string, absl::ParsedFormat<'s', 's'>>
    textproto_to_expected_format_string[] = {
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } "
      "selectors: { field: \"field1\" } }, "
      "predicate: { label: { semantic_tag: \"tag\"} }",
      absl::ParsedFormat<'s', 's'>("checkHasTag(\"%s.field1\", \"tag\") :- "
                               "mayHaveTag(\"%s.field1\", \"tag\").") },
    { "access_path: {"
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } }, "
      "predicate: { label: { semantic_tag: \"tag2\"} }",
      absl::ParsedFormat<'s', 's'>("checkHasTag(\"%s\", \"tag2\") :- "
                              "mayHaveTag(\"%s\", \"tag2\").") },
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" }, "
      "selectors: [{ field: \"x\" }, { field: \"y\" }] }, "
      "predicate: { label: { semantic_tag: \"user_selection\"} }",
      absl::ParsedFormat<'s', 's'>(
          "checkHasTag(\"%s.x.y\", \"user_selection\")" " :- "
          "mayHaveTag(\"%s.x.y\", \"user_selection\")" ".") }
};

INSTANTIATE_TEST_SUITE_P(
    TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(instantiated_roots)));

class TestTagCheckEquals : public testing::TestWithParam<
    std::tuple<
      std::tuple<AccessPath, std::string>,
      std::tuple<AccessPath, std::string>>> {};

TEST_P(TestTagCheckEquals, TestTagCheckEquals) {
  const std::tuple<AccessPath, std::string> tag_check_args1 =
      std::get<0>(GetParam());
  const std::tuple<AccessPath, std::string> tag_check_args2 =
    std::get<1>(GetParam());

  const TagCheck tag_check1(TagAnnotationOnAccessPath(
      std::get<0>(tag_check_args1), std::get<1>(tag_check_args1)));
  const TagCheck tag_check2(TagAnnotationOnAccessPath(
    std::get<0>(tag_check_args2), std::get<1>(tag_check_args2)));

  EXPECT_EQ(tag_check1 == tag_check2, tag_check_args1 == tag_check_args2);
}

static std::string sample_tags[] = {"tag1", "userSelection", "screen"};
static AccessPath sample_access_paths[] = {
    AccessPath(AccessPathRoot(
        HandleConnectionAccessPathRoot("recipe", "particle", "handle")),
               AccessPathSelectors(Selector(FieldSelector("field1")))),
    AccessPath(AccessPathRoot(
        HandleConnectionSpecAccessPathRoot("particle_spec", "handle_spec")),
               AccessPathSelectors(Selector(FieldSelector("field2")))) };

INSTANTIATE_TEST_SUITE_P(
    TestTagCheckEquals, TestTagCheckEquals,
    testing::Combine(
        testing::Combine(testing::ValuesIn(sample_access_paths),
                         testing::ValuesIn(sample_tags)),
        testing::Combine(testing::ValuesIn(sample_access_paths),
           testing::ValuesIn(sample_tags))));

}  // namespace raksha::ir
