#include "src/xform_to_datalog/arcs_manifest_tree/handle_connection_spec.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

namespace src::xform_to_datalog::arcs_manifest_tree {

struct ProtoStringAndExpectations {
  std::string proto_str;
  std::string expected_name;
  bool expected_reads;
  bool expected_writes;
};

class RoundTripHandleConnectionSpecProtoTest :
    public testing::TestWithParam<ProtoStringAndExpectations> {};

TEST_P(RoundTripHandleConnectionSpecProtoTest,
       RoundTripHandleConnectionSpecProtoTest) {
  ProtoStringAndExpectations info = GetParam();

  arcs::HandleConnectionSpecProto original_proto;
  google::protobuf::TextFormat::ParseFromString(
      info.proto_str, &original_proto);
  HandleConnectionSpec handle_connection_spec =
      HandleConnectionSpec::CreateFromProto(original_proto);

  EXPECT_EQ(handle_connection_spec.name(), info.expected_name);
  EXPECT_EQ(handle_connection_spec.reads(), info.expected_reads);
  EXPECT_EQ(handle_connection_spec.writes(), info.expected_writes);

  arcs::HandleConnectionSpecProto result_proto =
      handle_connection_spec.MakeProto();
   ASSERT_TRUE(
      google::protobuf::util::MessageDifferencer::Equals(
          original_proto, result_proto));
}

static const ProtoStringAndExpectations string_and_expections_array[] = {
    { .proto_str = "name: \"foo\" direction: READS", .expected_name = "foo",
      .expected_reads = true, .expected_writes = false},
    { .proto_str = "name: \"bar\" direction: WRITES", .expected_name = "bar",
      .expected_reads = false, .expected_writes = true },
    { .proto_str = "name: \"baz\" direction: READS_WRITES", .expected_name =
    "baz", .expected_reads = true, .expected_writes = true}
};

INSTANTIATE_TEST_SUITE_P(
    RoundTripHandleConnectionSpecProtoTest,
    RoundTripHandleConnectionSpecProtoTest,
    testing::ValuesIn(string_and_expections_array));

}  // namespace src::xform_to_datalog::arcs_manifest_tree
