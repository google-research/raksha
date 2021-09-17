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
#include "src/ir/proto/primitive_type.h"

#include <string>

#include "src/common/testing/gtest.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types::proto {

class PrimitiveTypeTest : public testing::TestWithParam<std::string> {};

TEST_P(PrimitiveTypeTest, RoundTripPreservesAllInformation) {
  const std::string &type_as_textproto = GetParam();
  arcs::PrimitiveTypeProto expected_proto;
  arcs::PrimitiveTypeProto_Parse(type_as_textproto, &expected_proto);
  EXPECT_EQ(expected_proto, encode(decode(expected_proto)));
}

TEST_P(PrimitiveTypeTest, RoundTripToTypeProtoAndBackWorks) {
  const std::string &type_as_textproto = GetParam();
  arcs::PrimitiveTypeProto expected_proto;
  arcs::PrimitiveTypeProto_Parse(type_as_textproto, &expected_proto);
  arcs::TypeProto type_proto = encodeAsTypeProto(decode(expected_proto));
  ASSERT_EQ(type_proto.data_case(), arcs::TypeProto::DataCase::kPrimitive);
  EXPECT_EQ(expected_proto, type_proto.primitive());
}

INSTANTIATE_TEST_SUITE_P(PrimitiveTypeTest, PrimitiveTypeTest,
                         testing::Values("TEXT"));

}  // namespace raksha::ir::types::proto
