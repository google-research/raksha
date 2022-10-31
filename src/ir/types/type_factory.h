//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
//----------------------------------------------------------------------------
#ifndef SRC_IR_TYPES_TYPE_FACTORY_H_
#define SRC_IR_TYPES_TYPE_FACTORY_H_

#include "absl/container/flat_hash_set.h"
#include "src/common/containers/hash_map.h"
#include "src/ir/types/primitive_type.h"
#include "src/ir/types/schema.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class Schema;

class TypeFactory {
 public:
  // Create a primitive type and wrap it in `types::Type`.
  Type MakePrimitiveType();

  // Create an entity type and wrap it in `types::Type`.
  Type MakeEntityType(const Schema& schema);

  // Create and register the given schema.
  const Schema& RegisterSchema(
      std::optional<std::string> name,
      common::containers::HashMap<std::string, Type> fields);

 private:
  // Known schemas.
  std::vector<std::unique_ptr<types::Schema>> schemas_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_TYPE_H_
