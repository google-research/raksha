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
#include "src/ir/types/proto/schema.h"

#include <optional>

#include "src/common/logging/logging.h"
#include "src/ir/types/proto/type.h"

namespace raksha::ir::types::proto {

using type::proto::SchemaProto;
using type::proto::TypeProto;

const ir::types::Schema &decode(ir::types::TypeFactory &type_factory,
                                const SchemaProto &schema_proto) {
  auto schema_names = schema_proto.names();
  CHECK(schema_names.size() <= 1)
      << "Multiple names for a Schema not yet supported.";
  std::optional<std::string> name;
  if (schema_names.size() == 1) {
    name = schema_names.at(0);
  }

  absl::flat_hash_map<std::string, ir::types::Type> field_map;
  for (const auto &field_name_type_pair : schema_proto.fields()) {
    const std::string &field_name = field_name_type_pair.first;
    const TypeProto &type_proto = field_name_type_pair.second;

    field_map.insert({field_name, Decode(type_factory, type_proto)});
  }

  return type_factory.RegisterSchema(std::move(name), std::move(field_map));
}

// Create an proto::SchemaProto message from the contents of this Schema object.
SchemaProto encode(const ir::types::Schema &schema) {
  SchemaProto schema_proto;
  if (const auto &name = schema.name()) {
    schema_proto.add_names(*name);
  }
  auto &fields_map = *schema_proto.mutable_fields();
  for (auto &field_name_type_pair : schema.fields()) {
    const std::string &field_name = field_name_type_pair.first;
    const ir::types::Type &field_type = field_name_type_pair.second;

    auto insert_result = fields_map.insert({field_name, Encode(field_type)});
    CHECK(insert_result.second)
        << "Found duplicate for field name " << field_name;
  }
  return schema_proto;
}

}  // namespace raksha::ir::types::proto
