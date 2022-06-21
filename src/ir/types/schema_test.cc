#include "src/ir/types/schema.h"

#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/proto/schema.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class ToStringTest
    : public testing::TestWithParam<std::tuple<std::string, std::string>> {
 protected:
  TypeFactory type_factory_;
};

// test the ToString() in schema.h
// takes as input a textproto to be converted to SchemaProto
// and the expected output of the ToString()
TEST_P(ToStringTest, ToStringTest) {
  const auto &[schema_as_textproto, expected_to_string_output] = GetParam();
  arcs::SchemaProto orig_schema_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(schema_as_textproto,
                                                            &orig_schema_proto))
      << "Failed to convert text to schema proto.";

  TypeFactory type_factory_;
  Schema schema = proto::decode(type_factory_, orig_schema_proto);

  EXPECT_THAT(schema.ToString(), expected_to_string_output);
}

// tuples of input for the ToStringTest test case
const std::tuple<std::string, std::string>
    schema_proto_and_to_string_output_strings[] = {
        // Schema with one primitive type.
        {R"(names: ["my_schema"]
            fields: [ { key: "field1", value: { primitive: TEXT } } ])",
         "schema my_schema {\n  field1: primitive\n}"},

        // Schema with no name and one primitive type
        {R"(fields: [ { key: "field1", value: { primitive: TEXT } } ])",
         "schema {\n  field1: primitive\n}"},

        // Schema with one primitive type and one entity
        {R"(names: ["my_schema"]
            fields: [ 
                { key: "field1", value: { primitive: TEXT } }, 
                { key: "field2", value: {entity: { schema: { } } 
                }}])",
         "schema my_schema {\n  field1: primitive\n  field2: entity\n}"},

        // Schema with no name and three fields: primitive, entity and primitive

        {R"(
            fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "x",
                value: {
                entity: { schema: { names: ["embedded"], fields: [
                    { key: "sub_field1", value: { primitive: TEXT } },
                    { key: "sub_field2", value: { primitive: TEXT } }
                ]}}}},
            { key: "hello", value: { primitive: TEXT } } ] )",
         "schema {\n  field1: primitive\n  hello: primitive\n  x: entity\n}"}};

INSTANTIATE_TEST_SUITE_P(
    ToStringTest, ToStringTest,
    testing::ValuesIn(schema_proto_and_to_string_output_strings));

}  // namespace raksha::ir::types
