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

#include "src/ir/derives_from_claim.h"

#include "google/protobuf/text_format.h"

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/ir/proto/access_path.h"
#include "src/ir/proto/derives_from_claim.h"

namespace raksha::ir {

// A DerivesFrom class is very simple structurally: it is just two
// AccessPaths smashed together. We take advantage of that to produce
// DerivesFrom textproto from pairs of AccessPath textprotos. We then use the
// pair to generate our observed output and the original AccessPath  textprotos
// to generate our expected output(s).
constexpr absl::string_view kDerivesFromTextprotoFormat =
    R"(target: {%s} source: {%s})";

static const std::string kSampleAccessPathTextprotos[] = {
    R"(
handle: { particle_spec: "ps", handle_connection: "hc" }
selectors: { field: "field1" })",
    R"(
handle: { particle_spec: "particle_spec", handle_connection: "handle_conn" }
selectors: [{ field: "x" }, { field: "y" }])",
    R"(handle: { particle_spec: "s", handle_connection: "h" })",
};

class DerivesFromAsAccessPathPairTest :
    public ::testing::TestWithParam<std::tuple<std::string, std::string>> {
 public:
  DerivesFromAsAccessPathPairTest()
    : derives_from_claim_(CreateDerivesFromFromParams()),
      expected_target_(CreateAccessPathFromParam<0>()),
      expected_source_(CreateAccessPathFromParam<1>()) {}

 protected:
  DerivesFromClaim CreateDerivesFromFromParams() const {
      arcs::ClaimProto_DerivesFrom derives_from_proto;
      const std::string &textproto =
          absl::StrFormat(
                kDerivesFromTextprotoFormat,
                std::get<0>(GetParam()),
                std::get<1>(GetParam()));
      google::protobuf::TextFormat::ParseFromString(
          textproto, &derives_from_proto);
      return proto::Decode(derives_from_proto);
  }

  template<int ParamNum>
  AccessPath CreateAccessPathFromParam() const {
    arcs::AccessPathProto access_path_proto;
    const std::string &textproto = std::get<ParamNum>(GetParam());
    google::protobuf::TextFormat::ParseFromString(
        textproto, &access_path_proto);
    return proto::Decode(access_path_proto);
  }

  DerivesFromClaim derives_from_claim_;
  AccessPath expected_target_;
  AccessPath expected_source_;
};

TEST_P(DerivesFromAsAccessPathPairTest, TargetTest) {
  EXPECT_EQ(derives_from_claim_.target(), expected_target_);
}

TEST_P(DerivesFromAsAccessPathPairTest, SourceTest) {
  EXPECT_EQ(derives_from_claim_.source(), expected_source_);
}

TEST_P(DerivesFromAsAccessPathPairTest, EdgeTest) {
  EXPECT_EQ(
      derives_from_claim_.GetAsEdge(),
      Edge(expected_source_, expected_target_));
}

INSTANTIATE_TEST_SUITE_P(
    DerivesFromAsAccessPathPairTest, DerivesFromAsAccessPathPairTest,
    testing::Combine(testing::ValuesIn(kSampleAccessPathTextprotos),
                     testing::ValuesIn(kSampleAccessPathTextprotos)));

}  // namespace raksha::ir
