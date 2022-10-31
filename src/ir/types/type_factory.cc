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
#include "src/ir/types/type_factory.h"

#include <memory>

#include "src/ir/types/entity_type.h"
#include "src/ir/types/primitive_type.h"
#include "src/ir/types/schema.h"

namespace raksha::ir::types {

Type TypeFactory::MakePrimitiveType() {
  return Type(std::make_unique<PrimitiveType>());
}

Type TypeFactory::MakeEntityType(const Schema& schema) {
  return Type(std::make_unique<EntityType>(schema));
}

const Schema& TypeFactory::RegisterSchema(
    std::optional<std::string> name,
    common::containers::HashMap<std::string, Type> fields) {
  std::unique_ptr<Schema> new_schema(
      new Schema(std::move(name), std::move(fields)));
  const Schema* result = new_schema.get();
  schemas_.push_back(std::move(new_schema));
  return *result;
}

}  // namespace raksha::ir::types
