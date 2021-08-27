#include "src/ir/tag_claim.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

class TagClaimToStringTest :
    public testing::TestWithParam<
      std::tuple<std::tuple<std::string, absl::string_view>, std::string>>
      {};

TEST_P(TagClaimToStringTest, TagClaimToStringTest) {
  const std::tuple<std::string, absl::string_view>
      &textproto_format_string_pair = std::get<0>(GetParam());
  const std::string &assume_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::string_view expected_tostring_format_string =
    std::get<1>(textproto_format_string_pair);
  const std::string &root_string = std::get<1>(GetParam());

  const std::string &expected_tostring = absl::StrFormat(
      expected_tostring_format_string, root_string);
  arcs::ClaimProto_Assume assume_proto;
  google::protobuf::TextFormat::ParseFromString(
      assume_textproto, &assume_proto);
  UnrootedTagClaim unrooted_tag_claim =
      UnrootedTagClaim::CreateFromProto(assume_proto);
  TagClaim tag_claim(root_string, unrooted_tag_claim);
  ASSERT_EQ(tag_claim.ToString(), expected_tostring);
}

// Sample root strings we can use to turn UnrootedTagClaims into TagClaims.
static std::string root_strings[] = {
    "recipe.handle",
    "recipe.particle",
    "r2.handle",
    "r2.particle"
};

// Pairs of textprotos and format strings. The format strings will become the
// expected ToString output when the root string is substituted for the %s.
// This allows us to test the result of combining each of the
// UnrootedTagClaims derived from the textprotos with each of the root strings.
static std::tuple<std::string, absl::string_view>
    textproto_to_expected_format_string[] = {
    { "access_path: { selectors: { field: \"field1\" } }, "
      "predicate: { label: { semantic_tag: \"tag\"} }",
      "claimHasTag(\"%s.field1\", \"tag\").\n" },
    { "access_path: { }, predicate: { label: { semantic_tag: \"tag2\"} }",
      "claimHasTag(\"%s\", \"tag2\").\n" },
    { "access_path: { selectors: [{ field: \"x\" }, { field: \"y\" }] }, "
      "predicate: { label: { semantic_tag: \"user_selection\"} }",
      "claimHasTag(\"%s.x.y\", \"user_selection\").\n" }
};

INSTANTIATE_TEST_SUITE_P(
    TagClaimToStringTest, TagClaimToStringTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(root_strings)));

}  // namespace raksha::ir
