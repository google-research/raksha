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
#include "src/ir/proto/access_path.h"

#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::proto {

class AccessPathFromProtoTest
    : public testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(AccessPathFromProtoTest, AccessPathFromProtoTest) {
  const auto& [textproto, expected_tostring_suffix] = GetParam();

  arcs::AccessPathProto access_path_proto;
  ASSERT_TRUE(google3_proto_compat::TextFormat::ParseFromString(textproto,
                                                            &access_path_proto))
      << "Unable to parse text proto.";

  AccessPath access_path = Decode(access_path_proto);

  AccessPathRoot root(
      HandleConnectionAccessPathRoot("recipe", "particle", "handle"));
  ASSERT_DEATH(
      access_path.ToString(),
      "Attempted to print out an AccessPath before connecting it to a "
      "fully-instantiated root!");
}

static const std::tuple<std::string, std::string>
    access_path_proto_tostring_pairs[]{
        {R"(handle: { particle_spec: "ps", handle_connection: "hc" })", ""},
        {R"(
handle: { particle_spec: "ps", handle_connection: "hc"}
selectors: { field: "foo" }
)",
         ".foo"},
        {R"(
handle: { particle_spec: "ps", handle_connection: "hc" }
selectors: [{ field: "foo" }, { field: "bar" }]
)",
         ".foo.bar"},
        {R"(
handle: { particle_spec: "ps", handle_connection: "hc" }
selectors: [{ field: "foo" }, { field: "bar" }, { field: "baz" }]
)",
         ".foo.bar.baz"}};

INSTANTIATE_TEST_SUITE_P(AccessPathFromProtoTest, AccessPathFromProtoTest,
                         testing::ValuesIn(access_path_proto_tostring_pairs));

}  // namespace raksha::ir::proto
