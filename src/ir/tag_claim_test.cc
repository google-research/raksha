#include "src/ir/tag_claim.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

class TagClaimToDatalogWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::string,
        std::tuple<std::string, absl::ParsedFormat<'s', 's'>>,
        AccessPathRoot>>
      {};

TEST_P(TagClaimToDatalogWithRootTest, TagClaimToDatalogWithRootTest) {
  const std::string particle_spec_name = std::get<0>(GetParam());
  const std::tuple<std::string, absl::ParsedFormat<'s', 's'>>
      &textproto_format_string_pair = std::get<1>(GetParam());
  const std::string &assume_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::ParsedFormat<'s', 's'> expected_todatalog_format_string =
    std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<2>(GetParam());

  const std::string &expected_todatalog = absl::StrFormat(
      expected_todatalog_format_string, particle_spec_name, root.ToString());
  arcs::ClaimProto_Assume assume_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      assume_textproto, &assume_proto));
  TagClaim unrooted_tag_claim =
      TagClaim::CreateFromProto(particle_spec_name, assume_proto);
  TagClaim tag_claim = unrooted_tag_claim.Instantiate(root);
  // Expect the version with the concrete root to match the expected_todatalog
  // when ToDatalog is called upon it.
  ASSERT_EQ(tag_claim.ToDatalog(), expected_todatalog);
  // However, the version with the spec root should fail when ToDatalog is
  // called.
  EXPECT_DEATH(
      unrooted_tag_claim.ToDatalog(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

// Sample particle spec names used in generating a TagClaim.
static std::string particle_spec_names[] = {
    "ParticleSpec1",
    "ParticleSpec2",
    "ps"
};

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
// expected ToDatalog output when the root string is substituted for the %s.
// This allows us to test the result of combining each of the
// TagClaims derived from the textprotos with each of the root strings.
static std::tuple<std::string, absl::ParsedFormat<'s', 's'>>
    textproto_to_expected_format_string[] = {
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { label: { semantic_tag: "tag"} })",
      absl::ParsedFormat<'s', 's'>(
          R"(claimHasTag("%s", "%s.field1", "tag").)") },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" } },
predicate: { label: { semantic_tag: "tag2"} })",
      absl::ParsedFormat<'s', 's'>(R"(claimHasTag("%s", "%s", "tag2").)") },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" },
  selectors: [{ field: "x" }, { field: "y" }] },
predicate: { label: { semantic_tag: "user_selection"} })",
      absl::ParsedFormat<'s', 's'>(
          R"(claimHasTag("%s", "%s.x.y", "user_selection").)") }
};

INSTANTIATE_TEST_SUITE_P(
    TagClaimToDatalogWithRootTest, TagClaimToDatalogWithRootTest,
    testing::Combine(
        testing::ValuesIn(particle_spec_names),
        testing::ValuesIn(textproto_to_expected_format_string),
        testing::ValuesIn(instantiated_roots)));

class TestTagClaimEquals : public testing::TestWithParam<
    std::tuple<
      std::tuple<std::string, AccessPath, std::string>,
      std::tuple<std::string, AccessPath, std::string>>> {};

TEST_P(TestTagClaimEquals, TestTagClaimEquals) {
  const std::tuple<std::string, AccessPath, std::string> tag_claim_args1 =
      std::get<0>(GetParam());
  const std::tuple<std::string, AccessPath, std::string> tag_claim_args2 =
    std::get<1>(GetParam());

  const TagClaim tag_claim1(
      std::get<0>(tag_claim_args1),
      TagAnnotationOnAccessPath(
          std::get<1>(tag_claim_args1), std::get<2>(tag_claim_args1)));
  const TagClaim tag_claim2(
      std::get<0>(tag_claim_args2),
      TagAnnotationOnAccessPath(
          std::get<1>(tag_claim_args2), std::get<2>(tag_claim_args2)));

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
        testing::Combine(
            testing::ValuesIn(particle_spec_names),
            testing::ValuesIn(sample_access_paths),
            testing::ValuesIn(sample_tags)),
        testing::Combine(
            testing::ValuesIn(particle_spec_names),
            testing::ValuesIn(sample_access_paths),
            testing::ValuesIn(sample_tags))));

}  // namespace raksha::ir
