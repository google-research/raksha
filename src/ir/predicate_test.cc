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
#include "src/ir/predicate_textproto_to_rule_body_testdata.h"
#include "src/ir/proto/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::predicates {

namespace proto = raksha::ir::proto;

class ToDatalogRuleBodyTest :
 public testing::TestWithParam<
  std::tuple<std::tuple<absl::string_view, absl::string_view>, AccessPath>> {
 public:
  explicit ToDatalogRuleBodyTest()
    : access_path_(std::get<1>(GetParam()))
  {
    const auto &[textproto, expected_rule_body_format] =
        std::get<0>(GetParam());

    textproto_ = textproto;
    expected_rule_body_ =
        absl::Substitute(expected_rule_body_format, access_path_.ToString());
  }

 protected:
  std::string textproto_;
  std::string expected_rule_body_;
  AccessPath access_path_;
};

TEST_P(ToDatalogRuleBodyTest, ToDatalogRuleBodyTest) {
  arcs::InformationFlowLabelProto_Predicate predicate_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      textproto_, &predicate_proto));
  std::unique_ptr<Predicate> predicate =
      proto::Decode(predicate_proto);
  EXPECT_EQ(
      predicate->ToDatalogRuleBody(access_path_), expected_rule_body_);
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

}  // namespace raksha::ir::predicates
