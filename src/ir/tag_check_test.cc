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
//----------------------------------------------------------------------------

#include "src/ir/tag_check.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/strings/str_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/noop_instantiator.h"
#include "src/ir/map_instantiator.h"

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
  const AccessPathRoot uninstantiated_root =
      AccessPathRoot(HandleConnectionSpecAccessPathRoot("ps", "hc"));
  const MapInstantiator kMapInstantiator(
      absl::flat_hash_map<AccessPathRoot, AccessPathRoot>{
        { uninstantiated_root, root }
      });

  std::string root_string = root.ToString(NoopInstantiator::Get());
  const std::string &expected_todatalog = absl::StrFormat(
      expected_todatalog_format_string, root_string, root_string);
  arcs::CheckProto check_proto;
  google::protobuf::TextFormat::ParseFromString(check_textproto, &check_proto);
  TagCheck unrooted_tag_check = TagCheck::CreateFromProto(check_proto);
  // Expect the version with an appropriate corresponding instantiation to
  // match the expected_todatalog when ToDatalog is called upon it.
  ASSERT_EQ(unrooted_tag_check.ToDatalog(kMapInstantiator), expected_todatalog);
  // However, the noop instantiator should fail when ToDatalog is called.
  EXPECT_DEATH(
      unrooted_tag_check.ToDatalog(NoopInstantiator::Get()),
      "Attempt to instantiate a non-instantiated root with the "
      "NoopInstantiator!");
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
