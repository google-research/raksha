//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
//----------------------------------------------------------------------------
#include "src/frontends/arcs/tag_check.h"

#include "absl/strings/str_format.h"
#include "absl/strings/substitute.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/arcs/proto/predicate.h"
#include "src/frontends/arcs/proto/tag_check.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::frontends::arcs {

class TagCheckToDatalogWithRootTest
    : public testing::TestWithParam<
          std::tuple<std::tuple<std::string, std::string>, AccessPathRoot>> {};

TEST_P(TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest) {
  const std::tuple<std::string, std::string> &textproto_format_string_pair =
      std::get<0>(GetParam());
  const std::string &check_textproto =
      std::get<0>(textproto_format_string_pair);
  const std::string_view expected_todatalog_format_string =
      std::get<1>(textproto_format_string_pair);
  const AccessPathRoot &root = std::get<1>(GetParam());
  std::string root_string = root.ToString();
  const std::string &expected_todatalog = absl::Substitute(
      expected_todatalog_format_string, "check_num_0", root_string);
  ::arcs::CheckProto check_proto;
  google::protobuf::TextFormat::ParseFromString(check_textproto, &check_proto);
  TagCheck unrooted_tag_check = proto::Decode(check_proto);

  DatalogPrintContext ctxt;
  DatalogPrintContext::AccessPathInstantiationMap instantiation_map(
      {{unrooted_tag_check.access_path().root(), root}});
  ctxt.set_instantiation_map(&instantiation_map);
  // Expect the version with the concrete root to match the expected_todatalog
  // when ToDatalog is called upon it.
  ASSERT_EQ(unrooted_tag_check.ToDatalog(ctxt), expected_todatalog);
  // However, the version with the spec root should fail when ToDatalog is
  // called.
  ctxt.set_instantiation_map(nullptr);
  EXPECT_DEATH(unrooted_tag_check.ToDatalog(ctxt),
               "Attempted to print out an AccessPath before connecting it to a "
               "fully-instantiated root!");
}

// Sample roots we can use to instantiate TagChecks.
static AccessPathRoot instantiated_roots[] = {
    AccessPathRoot(
        HandleConnectionAccessPathRoot("recipe", "particle", "handle")),
    AccessPathRoot(
        HandleConnectionAccessPathRoot("recipe2", "particle2", "handle2")),
    AccessPathRoot(HandleConnectionAccessPathRoot("r2", "p2", "h2")),
    AccessPathRoot(HandleConnectionAccessPathRoot("r2", "particle", "handle"))};

// Pairs of textprotos and format strings. The format strings will become the
// expected ToDatalog output when the root string is substituted for the %s.
// This allows us to test the result of combining each of the
// TagChecks derived from the textprotos with each of the root strings.
static std::tuple<std::string, std::string>
    textproto_to_expected_format_string[] = {
        {R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" }
      selectors: { field: "field1" } },
      predicate: { label: { semantic_tag: "tag"} })",
         R"(isCheck("$0", "$1.field1"). check("$0", owner, "$1.field1") :-
  ownsAccessPath(owner, "$1.field1"), mayHaveTag("$1.field1", owner, "tag").)"},
        {R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" } },
      predicate: { label: { semantic_tag: "tag2"} })",
         R"(isCheck("$0", "$1"). check("$0", owner, "$1") :-
  ownsAccessPath(owner, "$1"), mayHaveTag("$1", owner, "tag2").)"},
        {R"(access_path: {
      handle: { particle_spec: "ps", handle_connection: "hc" },
      selectors: [{ field: "x" }, { field: "y" }] },
      predicate: { label: { semantic_tag: "user_selection"} })",
         R"(isCheck("$0", "$1.x.y"). check("$0", owner, "$1.x.y") :-
  ownsAccessPath(owner, "$1.x.y"), mayHaveTag("$1.x.y", owner, "user_selection").)"}};

INSTANTIATE_TEST_SUITE_P(
    TagCheckToDatalogWithRootTest, TagCheckToDatalogWithRootTest,
    testing::Combine(testing::ValuesIn(textproto_to_expected_format_string),
                     testing::ValuesIn(instantiated_roots)));

}  // namespace raksha::frontends::arcs
