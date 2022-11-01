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
#ifndef SRC_IR_TYPES_TYPE_H_
#define SRC_IR_TYPES_TYPE_H_

#include "src/common/containers/hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/common/utils/intrusive_ptr.h"

namespace raksha::ir::types {

class TypeBase : public RefCounted<TypeBase> {
 public:
  enum class Kind { kPrimitive, kEntity };

  virtual ~TypeBase() {}

  virtual raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet()
      const = 0;

  // Returns the kind of type.
  virtual Kind kind() const = 0;
};

class Type {
 public:
  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const {
    return type_->GetAccessPathSelectorsSet();
  }

  // TODO: This should be removed to hide the TypeBase.  method is added as a
  // stop-gap solution to work with the rest of the code base.
  const TypeBase& type_base() const { return *type_.get(); }

  friend class TypeFactory;

 private:
  Type(std::unique_ptr<TypeBase> type) : type_(type.release()) {}

  intrusive_ptr<TypeBase> type_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_TYPE_H_
