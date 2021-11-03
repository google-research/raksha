#include "src/ir/tag_check.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/proto/predicate.h"
#include "src/ir/proto/tag_check.h"

namespace raksha::ir {

class TagCheckToDatalogWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::tuple<std::string, absl::ParsedFormat<'s'>>, AccessPathRoot>>
      {};

TEST_P(TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest) {
  const std::tuple<std::string, absl::ParsedFormat<'s'>>
      &textproto_format_string_pair = std::get<0>(GetParam());
  const std::string &check_textproto =
      std::get<0>(textproto_format_string_pair);
  const absl::ParsedFormat<'s'> expected_todatalog_format_string =
    std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<1>(GetParam());
  std::string root_string = root.ToString();
  const std::string &expected_todatalog = absl::StrFormat(
      expected_todatalog_format_string, root_string);
  arcs::CheckProto check_proto;
  google::protobuf::TextFormat::ParseFromString(check_textproto, &check_proto);
  PredicateArena predicate_arena;
  proto::PredicateDecoder predicate_decoder(predicate_arena);
  TagCheck unrooted_tag_check = proto::Decode(check_proto, predicate_decoder);
  TagCheck tag_check = unrooted_tag_check.Instantiate(root);

  DatalogPrintContext ctxt;
  // Expect the version with the concrete root to match the expected_todatalog
  // when ToDatalog is called upon it.
  ASSERT_EQ(tag_check.ToDatalog(ctxt), expected_todatalog);
  // However, the version with the spec root should fail when ToDatalog is
  // called.
  EXPECT_DEATH(
      unrooted_tag_check.ToDatalog(ctxt),
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
static std::tuple<std::string, absl::ParsedFormat<'s'>>
    textproto_to_expected_format_string[] = {
    { R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" }
      selectors: { field: "field1" } },
      predicate: { label: { semantic_tag: "tag"} })",
      absl::ParsedFormat<'s'>(
          R"(isCheck("check_num_0"). check("check_num_0") :- mayHaveTag("%s.field1", "tag").)") },
    { R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" } },
      predicate: { label: { semantic_tag: "tag2"} })",
      absl::ParsedFormat<'s'>(
          R"(isCheck("check_num_0"). check("check_num_0") :- mayHaveTag("%s", "tag2").)")
    },
    { R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" },
      selectors: [{ field: "x" }, { field: "y" }] },
      predicate: { label: { semantic_tag: "user_selection"} })",
      absl::ParsedFormat<'s'>(
          R"(isCheck("check_num_0"). check("check_num_0") :- mayHaveTag("%s.x.y", "user_selection").)") }
};

INSTANTIATE_TEST_SUITE_P(
    TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(instantiated_roots)));

}  // namespace raksha::ir
