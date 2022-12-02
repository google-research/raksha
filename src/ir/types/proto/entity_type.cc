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
#include "src/ir/types/proto/entity_type.h"

#include "src/ir/types/proto/schema.h"

namespace raksha::ir::types::proto {

Type decode(TypeFactory& type_factory,
            const EntityTypeProto& entity_type_proto) {
  CHECK(entity_type_proto.has_schema())
      << "Schema is required for Entity types.";
  return type_factory.MakeEntityType(
      decode(type_factory, entity_type_proto.schema()));
}

EntityTypeProto encode(const EntityType& entity_type) {
  EntityTypeProto entity_type_proto;
  *entity_type_proto.mutable_schema() = encode(entity_type.schema());
  return entity_type_proto;
}

TypeProto encodeAsTypeProto(const EntityType& entity_type) {
  TypeProto type_proto;
  *type_proto.mutable_entity() = encode(entity_type);
  return type_proto;
}

}  // namespace raksha::ir::types::proto
