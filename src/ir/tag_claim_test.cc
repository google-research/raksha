#include "src/ir/tag_claim.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

class TagClaimToStringWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::tuple<std::string, absl::ParsedFormat<'s'>>, AccessPathRoot>>
      {};

TEST_P(TagClaimToStringWithRootTest, TagClaimToStringWithRootTest) {
  const std::tuple<std::string, absl::ParsedFormat<'s'>>
      &textproto_format_string_pair = std::get<0>(GetParam());
  const std::string &assume_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::ParsedFormat<'s'> expected_tostring_format_string =
    std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<1>(GetParam());

  const std::string &expected_tostring = absl::StrFormat(
      expected_tostring_format_string, root.ToString());
  arcs::ClaimProto_Assume assume_proto;
  google::protobuf::TextFormat::ParseFromString(
      assume_textproto, &assume_proto);
  TagClaim unrooted_tag_claim =
      TagClaim::CreateFromProto(assume_proto);
  TagClaim tag_claim = unrooted_tag_claim.Instantiate(root);
  // Expect the version with the concrete root to match the expected_tostring
  // when ToString is called upon it.
  ASSERT_EQ(tag_claim.ToString(), expected_tostring);
  // However, the version with the spec root should fail when ToString is
  // called.
  EXPECT_DEATH(
      unrooted_tag_claim.ToString(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

// Sample roots we can use to instantiate TagClaims.
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
// TagClaims derived from the textprotos with each of the root strings.
static std::tuple<std::string, absl::ParsedFormat<'s'>>
    textproto_to_expected_format_string[] = {
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } "
      "selectors: { field: \"field1\" } }, "
      "predicate: { label: { semantic_tag: \"tag\"} }",
      absl::ParsedFormat<'s'>("claimHasTag(\"%s.field1\", \"tag\").\n") },
    { "access_path: {"
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" } }, "
      "predicate: { label: { semantic_tag: \"tag2\"} }",
      absl::ParsedFormat<'s'>("claimHasTag(\"%s\", \"tag2\").\n") },
    { "access_path: { "
      "handle: { particle_spec: \"ps\", " "handle_connection: \"hc\" }, "
      "selectors: [{ field: \"x\" }, { field: \"y\" }] }, "
      "predicate: { label: { semantic_tag: \"user_selection\"} }",
      absl::ParsedFormat<'s'>(
          "claimHasTag(\"%s.x.y\", \"user_selection\")" ".\n") }
};

INSTANTIATE_TEST_SUITE_P(
    TagClaimToStringWithRootTest, TagClaimToStringWithRootTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(instantiated_roots)));

class TestTagClaimEquals : public testing::TestWithParam<
    std::tuple<
      std::tuple<AccessPath, std::string>,
      std::tuple<AccessPath, std::string>>> {};

TEST_P(TestTagClaimEquals, TestTagClaimEquals) {
  const std::tuple<AccessPath, std::string> tag_claim_args1 =
      std::get<0>(GetParam());
  const std::tuple<AccessPath, std::string> tag_claim_args2 =
    std::get<1>(GetParam());

  const TagClaim tag_claim1(TagAnnotationOnAccessPath(
      std::get<0>(tag_claim_args1), std::get<1>(tag_claim_args1)));
  const TagClaim tag_claim2(TagAnnotationOnAccessPath(
    std::get<0>(tag_claim_args2), std::get<1>(tag_claim_args2)));

  EXPECT_EQ(tag_claim1 == tag_claim2, tag_claim_args1 == tag_claim_args2);
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
    TestTagClaimEquals, TestTagClaimEquals,
    testing::Combine(
        testing::Combine(testing::ValuesIn(sample_access_paths),
                         testing::ValuesIn(sample_tags)),
        testing::Combine(testing::ValuesIn(sample_access_paths),
           testing::ValuesIn(sample_tags))));

}  // namespace raksha::ir
