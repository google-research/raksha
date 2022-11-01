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

#include "src/frontends/arcs/predicate.h"

#include "absl/strings/str_replace.h"
#include "absl/strings/substitute.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/arcs/datalog_print_context.h"
#include "src/frontends/arcs/predicate_textproto_to_rule_body_testdata.h"
#include "src/frontends/arcs/proto/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs {

class ToDatalogRuleBodyTest
    : public testing::TestWithParam<std::tuple<
          std::tuple<std::string_view, std::string_view>, AccessPath>> {
 public:
  explicit ToDatalogRuleBodyTest() : access_path_(std::get<1>(GetParam())) {
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
};

TEST_P(ToDatalogRuleBodyTest, ToDatalogRuleBodyTest) {
  DatalogPrintContext ctxt;
  ::arcs::InformationFlowLabelProto_Predicate predicate_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(textproto_,
                                                            &predicate_proto));
  std::unique_ptr<Predicate> predicate = proto::Decode(predicate_proto);
  EXPECT_EQ(predicate->ToDatalogRuleBody(access_path_, ctxt),
            expected_rule_body_);
}

// Helper for making AccessPaths.
static ir::Selector MakeFieldSelector(std::string field_name) {
  return ir::Selector(ir::FieldSelector(field_name));
}

// Some sample access path textprotos. They aren't the main point of the test.
static const AccessPath sample_access_paths[] = {
    AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot("a", "b", "c")),
               ir::AccessPathSelectors(
                   MakeFieldSelector("d"),
                   ir::AccessPathSelectors(MakeFieldSelector("e"),
                                           ir::AccessPathSelectors()))),
    AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        ir::AccessPathSelectors()),
    AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot("foo", "bar", "baz")),
        ir::AccessPathSelectors(MakeFieldSelector("field"),
                                ir::AccessPathSelectors())),
};

// Note: look in predicate_textproto_to_rule_body_testdata for
// predicate_textproto_to_rule_body_format.
INSTANTIATE_TEST_SUITE_P(
    ToDatalogRuleBodyTest, ToDatalogRuleBodyTest,
    testing::Combine(testing::ValuesIn(predicate_textproto_to_rule_body_format),
                     testing::ValuesIn(sample_access_paths)));

class PredicateEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<const Predicate *, uint64_t>,
                     std::pair<const Predicate *, uint64_t>>> {
 protected:
  PredicateEqTest()
      : pred1_(std::get<0>(GetParam()).first),
        pred_num1(std::get<0>(GetParam()).second),
        pred2_(std::get<1>(GetParam()).first),
        pred_num2(std::get<1>(GetParam()).second) {}

  const Predicate *pred1_;
  uint64_t pred_num1;
  const Predicate *pred2_;
  uint64_t pred_num2;
};

// For this test, we create a number of expected-distinct predicates and
// number them. We make sure that the predicates compare equal only when they
// have the same number. We produce duplicates that are structurally the same
// but at different memory addresses to ensure that equality in the case of
// pointer equality is handled correctly.
TEST_P(PredicateEqTest, PredicateEqTest) {
  EXPECT_EQ(*pred1_ == *pred2_, pred_num1 == pred_num2);
}

static const TagPresence kTag1Present("tag1");
static const TagPresence kTag2Present("tag2");
static const TagPresence kTag3Present("tag3");

static const TagPresence kTag1Present2("tag1");
static const TagPresence kTag2Present2("tag2");
static const TagPresence kTag3Present2("tag3");

static const And kAndTag1Tag2(std::make_unique<TagPresence>("tag1"),
                              std::make_unique<TagPresence>("tag2"));
static const And kAndTag1Tag3(std::make_unique<TagPresence>("tag1"),
                              std::make_unique<TagPresence>("tag3"));
static const And kAndTag2Tag3(std::make_unique<TagPresence>("tag2"),
                              std::make_unique<TagPresence>("tag3"));

static const And kAndTag1Tag2_2(std::make_unique<TagPresence>("tag1"),
                                std::make_unique<TagPresence>("tag2"));
static const And kAndTag1Tag3_2(std::make_unique<TagPresence>("tag1"),
                                std::make_unique<TagPresence>("tag3"));
static const And kAndTag2Tag3_2(std::make_unique<TagPresence>("tag2"),
                                std::make_unique<TagPresence>("tag3"));

static const Or kOrTag1Tag2(std::make_unique<TagPresence>("tag1"),
                            std::make_unique<TagPresence>("tag2"));
static const Or kOrTag1Tag3(std::make_unique<TagPresence>("tag1"),
                            std::make_unique<TagPresence>("tag3"));
static const Or kOrTag2Tag3(std::make_unique<TagPresence>("tag2"),
                            std::make_unique<TagPresence>("tag3"));

static const Or kOrTag1Tag2_2(std::make_unique<TagPresence>("tag1"),
                              std::make_unique<TagPresence>("tag2"));
static const Or kOrTag1Tag3_2(std::make_unique<TagPresence>("tag1"),
                              std::make_unique<TagPresence>("tag3"));
static const Or kOrTag2Tag3_2(std::make_unique<TagPresence>("tag2"),
                              std::make_unique<TagPresence>("tag3"));

static const Implies kImpliesTag1Tag2(std::make_unique<TagPresence>("tag1"),
                                      std::make_unique<TagPresence>("tag2"));
static const Implies kImpliesTag1Tag2_2(std::make_unique<TagPresence>("tag1"),
                                        std::make_unique<TagPresence>("tag2"));
static const Implies kImpliesTag1Tag3(std::make_unique<TagPresence>("tag1"),
                                      std::make_unique<TagPresence>("tag3"));
static const Implies kImpliesTag1Tag3_2(std::make_unique<TagPresence>("tag1"),
                                        std::make_unique<TagPresence>("tag3"));
static const Implies kImpliesTag2Tag3(std::make_unique<TagPresence>("tag2"),
                                      std::make_unique<TagPresence>("tag3"));
static const Implies kImpliesTag2Tag3_2(std::make_unique<TagPresence>("tag2"),
                                        std::make_unique<TagPresence>("tag3"));

static const Not kNotTag1(std::make_unique<TagPresence>("tag1"));
static const Not kNotTag1_2(std::make_unique<TagPresence>("tag1"));
static const Not kNotTag2(std::make_unique<TagPresence>("tag2"));
static const Not kNotTag2_2(std::make_unique<TagPresence>("tag2"));
static const Not kNotTag3(std::make_unique<TagPresence>("tag3"));
static const Not kNotTag3_2(std::make_unique<TagPresence>("tag3"));

static const std::pair<const Predicate *, uint64_t> example_predicates[] = {
    {&kTag1Present, 0},      {&kTag1Present2, 0},
    {&kTag2Present, 1},      {&kTag2Present2, 1},
    {&kTag3Present, 2},      {&kTag3Present2, 2},
    {&kAndTag1Tag2, 3},      {&kAndTag1Tag2_2, 3},
    {&kAndTag1Tag3, 4},      {&kAndTag1Tag3_2, 4},
    {&kAndTag2Tag3, 5},      {&kAndTag2Tag3_2, 5},
    {&kOrTag1Tag2, 6},       {&kOrTag1Tag2_2, 6},
    {&kOrTag1Tag3, 7},       {&kOrTag1Tag3_2, 7},
    {&kOrTag2Tag3, 8},       {&kOrTag2Tag3_2, 8},
    {&kImpliesTag1Tag2, 9},  {&kImpliesTag1Tag2_2, 9},
    {&kImpliesTag1Tag3, 10}, {&kImpliesTag1Tag3_2, 10},
    {&kImpliesTag2Tag3, 11}, {&kImpliesTag2Tag3_2, 11},
    {&kNotTag1, 12},         {&kNotTag1_2, 12},
    {&kNotTag2, 13},         {&kNotTag2_2, 13},
    {&kNotTag3, 14},         {&kNotTag3_2, 14}};

INSTANTIATE_TEST_SUITE_P(
    PredicateEqTest, PredicateEqTest,
    testing::Combine(testing::ValuesIn(example_predicates),
                     testing::ValuesIn(example_predicates)));

}  // namespace raksha::frontends::arcs
