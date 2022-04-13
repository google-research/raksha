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

#include "src/ir/handle_connection_spec.h"

#include "google/protobuf/util/message_differencer.h"
#include "google/protobuf/text_format.h"

#include "src/common/testing/gtest.h"
#include "src/ir/proto/handle_connection_spec.h"
#include "src/ir/types/type_factory.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

struct ProtoStringAndExpectations {
  std::string proto_str;
  std::string expected_name;
  bool expected_reads;
  bool expected_writes;
};

class RoundTripHandleConnectionSpecProtoTest :
    public testing::TestWithParam<ProtoStringAndExpectations> {
protected:
  types::TypeFactory type_factory_;
};

TEST_P(RoundTripHandleConnectionSpecProtoTest,
       RoundTripHandleConnectionSpecProtoTest) {
  ProtoStringAndExpectations info = GetParam();

  arcs::HandleConnectionSpecProto original_proto;
  google::protobuf::TextFormat::ParseFromString(
      info.proto_str, &original_proto);
  HandleConnectionSpec handle_connection_spec =
      proto::Decode(type_factory_, original_proto);

  EXPECT_EQ(handle_connection_spec.name(), info.expected_name);
  EXPECT_EQ(handle_connection_spec.reads(), info.expected_reads);
  EXPECT_EQ(handle_connection_spec.writes(), info.expected_writes);

  arcs::HandleConnectionSpecProto result_proto =
      proto::Encode(handle_connection_spec);
   ASSERT_TRUE(
      google::protobuf::util::MessageDifferencer::Equals(
          original_proto, result_proto));
}

// Note: each of these protos has a type field that is just a TEXT primitive.
// Parsing the type correctly isn't the interesting part of parsing a
// HandleConnectionSpec; that is tested elsewhere. The type field is required
// and we are checking that it can be made back into an identical proto, so
// that suffices for this test.
static const ProtoStringAndExpectations string_and_expections_array[] = {
    { .proto_str =
        R"(name: "foo" direction: READS type: { primitive: TEXT })",
      .expected_name = "foo", .expected_reads = true, .expected_writes = false},
    { .proto_str =
        R"(name: "bar" direction: WRITES type: { primitive: TEXT })",
      .expected_name = "bar", .expected_reads = false,
      .expected_writes = true },
    { .proto_str =
        R"(name: "baz" direction: READS_WRITES type: { primitive: TEXT })",
      .expected_name = "baz", .expected_reads = true, .expected_writes = true}
};

INSTANTIATE_TEST_SUITE_P(
    RoundTripHandleConnectionSpecProtoTest,
    RoundTripHandleConnectionSpecProtoTest,
    testing::ValuesIn(string_and_expections_array));

struct ProtoStringAndExpectedAccessPathPieces {
  std::string textproto;
  std::string handle_connection_name;
  std::vector<std::string> access_path_selectors_tostrings;
};

class GetAccessPathTest :
 public testing::TestWithParam<
  std::tuple<ProtoStringAndExpectedAccessPathPieces, std::string>> {
 protected:
  types::TypeFactory type_factory_;
};

TEST_P(GetAccessPathTest, GetAccessPathTest) {
  const ProtoStringAndExpectedAccessPathPieces &param = std::get<0>(GetParam());
  const std::string &particle_spec_name = std::get<1>(GetParam());
  arcs::HandleConnectionSpecProto hcs_proto;
  google::protobuf::TextFormat::ParseFromString(param.textproto, &hcs_proto);
  HandleConnectionSpec handle_connection_spec =
      proto::Decode(type_factory_, hcs_proto);
  std::vector<ir::AccessPath> access_paths =
      handle_connection_spec.GetAccessPaths(particle_spec_name);
  std::vector<std::string> found_access_path_selector_strings;
  for (const ir::AccessPath &access_path : access_paths) {
    ir::AccessPathRoot root = access_path.root();
    EXPECT_EQ(
        root, ir::AccessPathRoot(ir::HandleConnectionSpecAccessPathRoot(
            particle_spec_name, param.handle_connection_name)));
    found_access_path_selector_strings.push_back(
        access_path.selectors().ToString());
  }
  EXPECT_THAT(found_access_path_selector_strings,
              testing::UnorderedElementsAreArray(
                  param.access_path_selectors_tostrings));
}

// Just some different strings that could be used as particle specs.
std::string sample_particle_spec_names[] = {
    "ParticleSpec",
    "PS",
    "P1",
    "P2"
};

ProtoStringAndExpectedAccessPathPieces protos_and_access_path_info[] = {
    { .textproto =
        R"(name: "handle_spec" direction: READS type: { primitive: TEXT })",
      .handle_connection_name = "handle_spec",
      .access_path_selectors_tostrings = { "" } },
    { .textproto = R"(
name: "hs" direction: READS_WRITES
type: { entity: { schema: { fields [
  { key: "field1", value: { primitive: TEXT } } ] } } })",
         .handle_connection_name = "hs",
         .access_path_selectors_tostrings = { ".field1" }},
    { .textproto = R"(
name: "spechandle" direction: WRITES
type { entity: { schema: { fields: [
  { key: "field1", value: { primitive: TEXT } },
  { key: "x", value: {
      entity: { schema: { names: ["embedded"], fields: [
        { key: "sub_field1", value: { primitive: TEXT } },
        { key: "sub_field2", value: { primitive: TEXT } } ]}}}},
  { key: "hello", value: { primitive: TEXT } } ] } } })",
      .handle_connection_name = "spechandle",
      .access_path_selectors_tostrings = {
        ".field1", ".x.sub_field1", ".x.sub_field2", ".hello"
    } }
};

INSTANTIATE_TEST_SUITE_P(
    GetAccessPathTest, GetAccessPathTest,
    testing::Combine(testing::ValuesIn(protos_and_access_path_info),
                     testing::ValuesIn(sample_particle_spec_names)));

}  // namespace raksha::ir
