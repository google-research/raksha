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
#include "src/ir/single-use-arena-and-predicate.h"
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
    std::string may_have_subst = absl::StrFormat(
        R"(ownsAccessPath(owner, "%s"), mayHaveTag("%s", owner)",
        access_path_.ToString(), access_path_.ToString());
    expected_rule_body_ =
        absl::Substitute(expected_rule_body_format, may_have_subst);
  }

 protected:
  std::string textproto_;
  std::string expected_rule_body_;
  AccessPath access_path_;
  PredicateArena arena_;
  proto::PredicateDecoder predicate_decoder_;
};

TEST_P(ToDatalogRuleBodyTest, ToDatalogRuleBodyTest) {
  arcs::InformationFlowLabelProto_Predicate predicate_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      textproto_, &predicate_proto));
  const Predicate &predicate = *predicate_decoder_.Decode(predicate_proto);
  EXPECT_EQ(
      predicate.ToDatalogRuleBody(access_path_), expected_rule_body_);
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

class PredicateEqTest :
 public testing::TestWithParam<
     std::tuple<std::pair<const Predicate *, uint64_t>,
                std::pair<const Predicate *, uint64_t>>> {
  protected:
  PredicateEqTest()
  : pred1_(std::get<0>(GetParam()).first),
    pred_num1(std::get<0>(GetParam()).second),
    pred2_(std::get<1>(GetParam()).first),
    pred_num2(std::get<1>(GetParam()).second) { }

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

static const SingleUseArenaAndTagPresence kTag1Present("tag1");
static const SingleUseArenaAndTagPresence kTag2Present("tag2");
static const SingleUseArenaAndTagPresence kTag3Present("tag3");

static const SingleUseArenaAndTagPresence kTag1Present2("tag1");
static const SingleUseArenaAndTagPresence kTag2Present2("tag2");
static const SingleUseArenaAndTagPresence kTag3Present2("tag3");

static const SingleUseArenaAndAnd kAndTag1Tag2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndAnd kAndTag1Tag3(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndAnd kAndTag2Tag3(
    kTag2Present.predicate(), kTag3Present.predicate());

static const SingleUseArenaAndAnd kAndTag1Tag2_2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndAnd kAndTag1Tag3_2(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndAnd kAndTag2Tag3_2(
    kTag2Present.predicate(), kTag3Present.predicate());

static const SingleUseArenaAndOr kOrTag1Tag2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndOr kOrTag1Tag3(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndOr kOrTag2Tag3(
    kTag2Present.predicate(), kTag3Present.predicate());

static const SingleUseArenaAndOr kOrTag1Tag2_2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndOr kOrTag1Tag3_2(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndOr kOrTag2Tag3_2(
    kTag2Present.predicate(), kTag3Present.predicate());

static const SingleUseArenaAndImplies kImpliesTag1Tag2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndImplies kImpliesTag1Tag2_2(
    kTag1Present.predicate(), kTag2Present.predicate());
static const SingleUseArenaAndImplies kImpliesTag1Tag3(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndImplies kImpliesTag1Tag3_2(
    kTag1Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndImplies kImpliesTag2Tag3(
    kTag2Present.predicate(), kTag3Present.predicate());
static const SingleUseArenaAndImplies kImpliesTag2Tag3_2(
    kTag2Present.predicate(), kTag3Present.predicate());

static const SingleUseArenaAndNot kNotTag1(kTag1Present.predicate());
static const SingleUseArenaAndNot kNotTag1_2(kTag1Present.predicate());
static const SingleUseArenaAndNot kNotTag2(kTag2Present.predicate());
static const SingleUseArenaAndNot kNotTag2_2(kTag2Present.predicate());
static const SingleUseArenaAndNot kNotTag3(kTag3Present.predicate());
static const SingleUseArenaAndNot kNotTag3_2(kTag3Present.predicate());

static const std::pair<const Predicate *, uint64_t> example_predicates[] = {
    { kTag1Present.predicate(), 0},
    { kTag1Present2.predicate(), 0},
    { kTag2Present.predicate(), 1},
    { kTag2Present2.predicate(), 1},
    { kTag3Present.predicate(), 2},
    { kTag3Present2.predicate(), 2},
    { kAndTag1Tag2.predicate(), 3},
    { kAndTag1Tag2_2.predicate(), 3},
    { kAndTag1Tag3.predicate(), 4},
    { kAndTag1Tag3_2.predicate(), 4},
    { kAndTag2Tag3.predicate(), 5},
    { kAndTag2Tag3_2.predicate(), 5},
    { kOrTag1Tag2.predicate(), 6},
    { kOrTag1Tag2_2.predicate(), 6},
    { kOrTag1Tag3.predicate(), 7},
    { kOrTag1Tag3_2.predicate(), 7},
    { kOrTag2Tag3.predicate(), 8},
    { kOrTag2Tag3_2.predicate(), 8},
    { kImpliesTag1Tag2.predicate(), 9},
    { kImpliesTag1Tag2_2.predicate(), 9},
    { kImpliesTag1Tag3.predicate(), 10},
    { kImpliesTag1Tag3_2.predicate(), 10},
    { kImpliesTag2Tag3.predicate(), 11},
    { kImpliesTag2Tag3_2.predicate(), 11},
    { kNotTag1.predicate(), 12},
    { kNotTag1_2.predicate(), 12},
    { kNotTag2.predicate(), 13},
    { kNotTag2_2.predicate(), 13},
    { kNotTag3.predicate(), 14},
    { kNotTag3_2.predicate(), 14}
};

INSTANTIATE_TEST_SUITE_P(
    PredicateEqTest, PredicateEqTest,
    testing::Combine(
        testing::ValuesIn(example_predicates),
        testing::ValuesIn(example_predicates)));

}  // namespace raksha::ir
