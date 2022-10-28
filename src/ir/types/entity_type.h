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
//----------------------------------------------------------------------------
#ifndef SRC_IR_TYPES_ENTITY_TYPE_H_
#define SRC_IR_TYPES_ENTITY_TYPE_H_

#include "absl/hash/hash.h"
#include "src/common/absl_shim/container/flat_hash_set.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/schema.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class EntityType : public TypeBase {
 public:
  explicit EntityType(const Schema& schema) : schema_(&schema) {}

  TypeBase::Kind kind() const override { return TypeBase::Kind::kEntity; }

  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet()
      const override {
    return schema_->GetAccessPathSelectorsSet();
  }

  const Schema& schema() const { return *schema_; }

 private:
  const Schema* schema_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_ENTITY_TYPE_H_
