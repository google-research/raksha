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
#ifndef SRC_IR_TYPES_PRIMITIVE_TYPE_H_
#define SRC_IR_TYPES_PRIMITIVE_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

// TODO: Add the various kinds of primitive type.
// Right now, the most important thing about primitive types is that they
// terminate an access path. For now, we've left them empty; we should fill
// them in in the future.
class PrimitiveType : public TypeBase {
 public:
  // For now, a primitive type has no members and a trivial constructor. This
  // will change as we add more cases to this translator in the future.
  PrimitiveType() = default;

  TypeBase::Kind kind() const override { return TypeBase::Kind::kPrimitive; }

  // A primitive type marks the end of a single access path to be built.
  // Return a set containing an empty AccessPathSelectors object.
  raksha::ir::AccessPathSelectorsSet
    GetAccessPathSelectorsSet() const override {
    return raksha::ir::AccessPathSelectorsSet(
        {raksha::ir::AccessPathSelectors()});
  }
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_PRIMITIVE_TYPE_H_
