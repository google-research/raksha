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
#ifndef SRC_IR_TYPES_PROTO_ENTITY_TYPE_H_
#define SRC_IR_TYPES_PROTO_ENTITY_TYPE_H_

#include "src/ir/proto/raksha_type.pb.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir::types::proto {

using type::proto::EntityTypeProto;
using type::proto::TypeProto;

// Decodes the given `entity_type_proto` as an EntityType.
Type decode(TypeFactory& type_factory,
            const EntityTypeProto& entity_type_proto);

// Encodes the given `entity_type` as an EntityTypeProto.
EntityTypeProto encode(const EntityType& entity_type);

// Returns a TypeProto with the given entity_type.
TypeProto encodeAsTypeProto(const EntityType& entity_type);

}  // namespace raksha::ir::types::proto

#endif  // SRC_IR_TYPES_PROTO_ENTITY_TYPE_H_
