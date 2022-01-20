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
#include "src/ir/proto/entity_type.h"

#include <string>

#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/ir/proto/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types::proto {

class EntityTypeTest : public testing::TestWithParam<std::string> {};

TEST_P(EntityTypeTest, RoundTripPreservesAllInformation) {
  arcs::TypeProto orig_type_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(GetParam(),
                                                            &orig_type_proto))
      << "Failed to parse type proto.";
  arcs::TypeProto result_type_proto = Encode(Decode(orig_type_proto));
  ASSERT_TRUE(google::protobuf::util::MessageDifferencer::Equals(
      orig_type_proto, result_type_proto));
}

INSTANTIATE_TEST_SUITE_P(
    EntityTypeTest, EntityTypeTest,
    testing::Values(
        // Simple primitive type.
        R"(primitive: TEXT)",
        // Entity with no fields.
        R"(entity: { schema: { } })",
        // Entity with one primitive field, field1.
        R"(
entity: {
  schema: {
    fields [ { key: "field1", value: { primitive: TEXT } } ]
  }
}
)",
        // Entity with one primitive field and a named schema
        R"(
entity: {
  schema: {
    names: ["my_schema"]
    fields: [ { key: "field1", value: { primitive: TEXT } } ]
  }
}
)",
        // Entity with multiple primitive fields.
        R"(
entity: {
  schema: {
    fields: [
      { key: "field1", value: { primitive: TEXT } },
      { key: "x", value: { primitive: TEXT } },
      { key: "hello", value: { primitive: TEXT } }
    ]
  }
}
)",
        // Entity with sub entities and primitive fields.
        R"(
entity: {
  schema: {
    fields: [
      { key: "field1", value: { primitive: TEXT } },
      { key: "x",
        value: {
          entity: {
            schema: {
              names: ["embedded"],
              fields: [
                { key: "sub_field1", value: { primitive: TEXT } },
                { key: "sub_field2", value: { primitive: TEXT } }
              ]
            }
          }
        }
      },
      { key: "hello", value: { primitive: TEXT } }
    ]
  }
}
)"));

}  // namespace raksha::ir::types::proto
