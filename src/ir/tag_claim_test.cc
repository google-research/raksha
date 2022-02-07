//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

#include "src/ir/tag_claim.h"

#include "google/protobuf/util/message_differencer.h"
#include "google/protobuf/text_format.h"

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/proto/tag_claim.h"

namespace raksha::ir {

class TagClaimToDatalogWithRootTest :
    public testing::TestWithParam<
      std::tuple<
        std::string,
        std::tuple<std::string, std::vector<absl::ParsedFormat<'s', 's', 's'>>>,
        AccessPathRoot>>
      {};

TEST_P(TagClaimToDatalogWithRootTest, TagClaimToDatalogWithRootTest) {
  const std::string particle_spec_name = std::get<0>(GetParam());
  const std::tuple<std::string, std::vector<absl::ParsedFormat<'s', 's', 's'>>>
      &textproto_format_string_pair = std::get<1>(GetParam());
  const std::string &assume_textproto =
      std::get<0>(textproto_format_string_pair);
  const std::vector<absl::ParsedFormat<'s', 's', 's'>>
      expected_todatalog_format_string_vec =
          std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<2>(GetParam());

  std::vector<std::string> expected_todatalog_vec;
  for (auto &format_str : expected_todatalog_format_string_vec) {
    expected_todatalog_vec.push_back(
        absl::StrFormat(format_str, particle_spec_name,
                        root.ToString(), root.ToString()));
  }
  arcs::ClaimProto_Assume assume_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      assume_textproto, &assume_proto));
  std::vector<TagClaim> unrooted_tag_claim_vec =
      proto::Decode(particle_spec_name, assume_proto);
  DatalogPrintContext::AccessPathInstantiationMap instantiation_map;
  for (const TagClaim &unrooted_claim : unrooted_tag_claim_vec) {
    instantiation_map.insert({unrooted_claim.access_path().root(), root});
  }
  DatalogPrintContext ctxt;
  // Expect the version with the concrete root to match the expected_todatalog
  // when ToDatalog is called upon it.
  ctxt.set_instantiation_map(&instantiation_map);
  ASSERT_EQ(unrooted_tag_claim_vec.size(), expected_todatalog_vec.size());
  for (uint64_t i = 0; i < unrooted_tag_claim_vec.size(); ++i) {
    EXPECT_EQ(
        unrooted_tag_claim_vec.at(i).ToDatalog(ctxt),
        expected_todatalog_vec.at(i));
  }
  // However, the version with the spec root should fail when ToDatalog is
  // called.
  ctxt.set_instantiation_map(nullptr);
  EXPECT_DEATH(
      unrooted_tag_claim_vec.at(0).ToDatalog(ctxt),
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
static std::tuple<std::string, std::vector<absl::ParsedFormat<'s', 's', 's'>>>
    textproto_to_expected_format_string[] = {
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { label: { semantic_tag: "tag"} })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag") :- ownsAccessPath(owner, "%s.field1").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" } },
predicate: { label: { semantic_tag: "tag2"} })",
      { absl::ParsedFormat<'s', 's', 's'>(R"(says_hasTag("%s", "%s", owner, "tag2") :- ownsAccessPath(owner, "%s").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" },
  selectors: [{ field: "x" }, { field: "y" }] },
predicate: { label: { semantic_tag: "user_selection"} })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.x.y", owner, "user_selection") :- ownsAccessPath(owner, "%s.x.y").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" },
  selectors: [{ field: "x" }, { field: "y" }] },
predicate: {
not: { predicate: { label: { semantic_tag: "user_selection"} } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s.x.y", owner, "user_selection") :- ownsAccessPath(owner, "%s.x.y").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { and: {
  conjunct0: { label: { semantic_tag: "tag"} }
  conjunct1: { label: { semantic_tag: "tag2"} } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag2") :- ownsAccessPath(owner, "%s.field1").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { and: {
  conjunct0: { not: { predicate: { label: { semantic_tag: "tag"} } } }
  conjunct1: { label: { semantic_tag: "tag2"} } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s.field1", owner, "tag") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag2") :- ownsAccessPath(owner, "%s.field1").)") } },
    { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" } },
predicate: { and: {
  conjunct0: { label: { semantic_tag: "tag"} }
  conjunct1: { not: { predicate: { label: { semantic_tag: "tag2"} } } } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s", owner, "tag") :- ownsAccessPath(owner, "%s").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s", owner, "tag2") :- ownsAccessPath(owner, "%s").)") } },
     { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { and: {
  conjunct0: { and: {
    conjunct0: { label: { semantic_tag: "tag"} }
    conjunct1: { not: { predicate: { label: { semantic_tag: "tag2"} } } } } }
  conjunct1: { label: { semantic_tag: "tag3"} } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s.field1", owner, "tag2") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag3") :- ownsAccessPath(owner, "%s.field1").)") } },
     { R"(
access_path: {
  handle: { particle_spec: "ps", handle_connection: "hc" }
  selectors: { field: "field1" } },
predicate: { and: {
  conjunct0: { and: {
    conjunct0: { label: { semantic_tag: "tag"} }
    conjunct1: { not: { predicate: { label: { semantic_tag: "tag2"} } } } } }
  conjunct1: { and: {
    conjunct0: { not: { predicate: { label: { semantic_tag: "tag3"} } } }
    conjunct1: { label: { semantic_tag: "tag4" } } } } } })",
      { absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s.field1", owner, "tag2") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_removeTag("%s", "%s.field1", owner, "tag3") :- ownsAccessPath(owner, "%s.field1").)"),
        absl::ParsedFormat<'s', 's', 's'>(
          R"(says_hasTag("%s", "%s.field1", owner, "tag4") :- ownsAccessPath(owner, "%s.field1").)")
          } },
};

INSTANTIATE_TEST_SUITE_P(
    TagClaimToDatalogWithRootTest, TagClaimToDatalogWithRootTest,
    testing::Combine(
        testing::ValuesIn(particle_spec_names),
        testing::ValuesIn(textproto_to_expected_format_string),
        testing::ValuesIn(instantiated_roots)));

class TestTagClaimEquals : public testing::TestWithParam<
    std::tuple<
      std::tuple<std::string, AccessPath, bool, std::string>,
      std::tuple<std::string, AccessPath, bool, std::string>>> {};

TEST_P(TestTagClaimEquals, TestTagClaimEquals) {
  const std::tuple<std::string, AccessPath, bool, std::string> tag_claim_args1 =
      std::get<0>(GetParam());
  const std::tuple<std::string, AccessPath, bool, std::string> tag_claim_args2 =
    std::get<1>(GetParam());

  const TagClaim tag_claim1(
      std::get<0>(tag_claim_args1), std::get<1>(tag_claim_args1),
      std::get<2>(tag_claim_args1), std::get<3>(tag_claim_args1));
  const TagClaim tag_claim2(
    std::get<0>(tag_claim_args2), std::get<1>(tag_claim_args2),
    std::get<2>(tag_claim_args2), std::get<3>(tag_claim_args2));

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
            testing::Values(true, false),
            testing::ValuesIn(sample_tags)),
        testing::Combine(
            testing::ValuesIn(particle_spec_names),
            testing::ValuesIn(sample_access_paths),
            testing::Values(true, false),
            testing::ValuesIn(sample_tags))));

}  // namespace raksha::ir
