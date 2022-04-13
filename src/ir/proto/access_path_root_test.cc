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

class AccessPathRootTest : public testing::TestWithParam<std::string> {};

TEST_P(AccessPathRootTest, RoundTripToProtoPreservesAll) {
  arcs::AccessPathProto_HandleRoot orig_root_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(GetParam(),
                                                            &orig_root_proto))
      << "Unable to parse text proto.";
  ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
      orig_root_proto, Encode(Decode(orig_root_proto))));
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathRootTest, AccessPathRootTest,
    testing::Values(R"(particle_spec: "ps", handle_connection: "hc")",
                    R"(particle_spec: "abc", handle_connection: "def")"));

}  // namespace raksha::ir::proto
