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

#include "src/ir/predicate.h"

#include "absl/strings/str_replace.h"
#include "absl/strings/substitute.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/datalog_print_context.h"
#include "src/ir/predicate_textproto_to_rule_body_testdata.h"
#include "src/ir/proto/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

class ToDatalogRuleBodyTest :
 public testing::TestWithParam<
  std::tuple<std::tuple<absl::string_view, absl::string_view>, AccessPath>> {
 public:
  explicit ToDatalogRuleBodyTest()
    : access_path_(std::get<1>(GetParam())),
      predicate_decoder_(arena_)
  {
    const auto &[textproto, expected_rule_body_format] =
        std::get<0>(GetParam());

    textproto_ = textproto;
    expected_rule_body_ = absl::Substitute(expected_rule_body_format,
                                           access_path_.ToString(), "owner");
  }

 protected:
  std::string textproto_;
  std::string expected_rule_body_;
  AccessPath access_path_;
  PredicateArena arena_;
  proto::PredicateDecoder predicate_decoder_;
};

TEST_P(ToDatalogRuleBodyTest, ToDatalogRuleBodyTest) {
  DatalogPrintContext ctxt;
  arcs::InformationFlowLabelProto_Predicate predicate_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      textproto_, &predicate_proto));
  std::unique_ptr<Predicate> predicate =
      predicate_decoder_.Decode(predicate_proto);
  EXPECT_EQ(
      predicate->ToDatalogRuleBody(access_path_, ctxt), expected_rule_body_);
}

// Helper for making AccessPaths.
static Selector MakeFieldSelector(std::string field_name) {
  return Selector(FieldSelector(field_name));
}

// Some sample access path textprotos. They aren't the main point of the test.
static const AccessPath sample_access_paths[] = {
    AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot("a", "b", "c")),
             AccessPathSelectors(
                 MakeFieldSelector("d"),
                 AccessPathSelectors(
                     MakeFieldSelector("e"), AccessPathSelectors()))),
    AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        AccessPathSelectors()),
    AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        AccessPathSelectors(
            MakeFieldSelector("field"), AccessPathSelectors())),
};

// Note: look in predicate_textproto_to_rule_body_testdata for
// predicate_textproto_to_rule_body_format.
INSTANTIATE_TEST_SUITE_P(ToDatalogRuleBodyTest, ToDatalogRuleBodyTest,
                         testing::Combine(testing::ValuesIn(
                             predicate_textproto_to_rule_body_format),
                         testing::ValuesIn(sample_access_paths)));

class PredicateEqTest : public testing::TestWithParam<
                            std::tuple<const Predicate *, const Predicate *>> {
};

// For this test, we create a number of expected-distinct predicates and try
// all pairs of them. We then ensure that all of these predicates are equal
// only when their pointers are equal.
TEST_P(PredicateEqTest, PredicateEqTest) {
  const Predicate *pred1 = std::get<0>(GetParam());
  const Predicate *pred2 = std::get<1>(GetParam());

  EXPECT_EQ(pred1 == pred2, *pred1 == *pred2);
}

static const TagPresence kTag1Present("tag1");
static const TagPresence kTag2Present("tag2");
static const TagPresence kTag3Present("tag3");

static const And kAndTag1Tag2(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag2"));
static const And kAndTag1Tag3(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag3"));
static const And kAndTag2Tag3(
    std::make_unique<TagPresence>("tag2"), std::make_unique<TagPresence>("tag3"));

static const Or kOrTag1Tag2(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag2"));
static const Or kOrTag1Tag3(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag3"));
static const Or kOrTag2Tag3(
    std::make_unique<TagPresence>("tag2"), std::make_unique<TagPresence>("tag3"));

static const Implies kImpliesTag1Tag2(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag2"));
static const Implies kImpliesTag1Tag3(
    std::make_unique<TagPresence>("tag1"), std::make_unique<TagPresence>("tag3"));
static const Implies kImpliesTag2Tag3(
    std::make_unique<TagPresence>("tag2"), std::make_unique<TagPresence>("tag3"));

static const Not kNotTag1(std::make_unique<TagPresence>("tag1"));
static const Not kNotTag1_2(std::make_unique<TagPresence>("tag1"));
static const Not kNotTag2(std::make_unique<TagPresence>("tag2"));
static const Not kNotTag2_2(std::make_unique<TagPresence>("tag2"));
static const Not kNotTag3(std::make_unique<TagPresence>("tag3"));
static const Not kNotTag3_2(std::make_unique<TagPresence>("tag3"));

static const Predicate * example_predicate_pointers[] = {
  &kTag1Present,
  &kTag2Present,
  &kTag3Present,
  &kAndTag1Tag2,
  &kAndTag1Tag3,
  &kAndTag2Tag3,
  &kOrTag1Tag2,
  &kOrTag1Tag3,
  &kOrTag2Tag3,
  &kImpliesTag1Tag2,
  &kImpliesTag1Tag3,
  &kImpliesTag2Tag3,
  &kNotTag1,
  &kNotTag2,
  &kNotTag3,
};

INSTANTIATE_TEST_SUITE_P(
    PredicateEqTest, PredicateEqTest,
    testing::Combine(
        testing::ValuesIn(example_predicate_pointers),
        testing::ValuesIn(example_predicate_pointers)));


}  // namespace raksha::ir
