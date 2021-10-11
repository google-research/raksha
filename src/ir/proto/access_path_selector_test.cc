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
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/ir/proto/access_path.h"

namespace raksha::ir::proto {

class AccessPathSelectorTest : public testing::TestWithParam<std::string> {};

TEST_P(AccessPathSelectorTest, RoundTripToProtoPreservesAll) {
 arcs::AccessPathProto_Selector orig_selector_proto;
 ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
     GetParam(), &orig_selector_proto))
     << "Unable to parse text proto.";
 ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
     orig_selector_proto, Encode(Decode(orig_selector_proto))));
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathSelectorTest, AccessPathSelectorTest,
    testing::Values(R"(field: "foo")", R"(field: "x")", R"(field: "bar")"));


class AccessPathSelectorsFromProtoTest
 : public testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(AccessPathSelectorsFromProtoTest, AccessPathSelectorsFromProtoTest) {
  const auto& [access_path_textproto, expected_to_string] = GetParam();

  arcs::AccessPathProto orig_access_path_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      access_path_textproto, &orig_access_path_proto))
      << "Unable to parse text proto.";

  // If this test were given an AccessPathProto with a root element, it would
  // fail because this class handles the selector piece only. Guard ourselves
  // from confusion due to bad inputs by checking that our input does not
  // have a root.
  EXPECT_FALSE(orig_access_path_proto.has_handle());
  EXPECT_FALSE(orig_access_path_proto.has_store_id());

  AccessPathSelectors access_path_selectors =
      DecodeSelectors(orig_access_path_proto);
  EXPECT_EQ(access_path_selectors.ToString(), expected_to_string);

  arcs::AccessPathProto result_access_path_proto;
  EncodeSelectors(access_path_selectors, result_access_path_proto);
  EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
      result_access_path_proto, orig_access_path_proto));
}

static const std::tuple<std::string, std::string>
    access_path_selectors_proto_tostring_pairs[]{
        {"", ""},
        {R"(selectors: { field: "foo" })", ".foo"},
        {R"(selectors: [{ field: "foo" }, { field: "bar" }])", ".foo.bar"},
        {R"(selectors: [{ field: "foo" }, { field: "bar" }, { field: "baz" }])",
         ".foo.bar.baz"},
    };

INSTANTIATE_TEST_SUITE_P(
    AccessPathSelectorsFromProtoTest, AccessPathSelectorsFromProtoTest,
    testing::ValuesIn(access_path_selectors_proto_tostring_pairs));



}  // namespace raksha::ir::proto
