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
#ifndef SRC_IR_ATTRIBUTES_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTES_ATTRIBUTE_H_

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/common/utils/intrusive_ptr.h"

namespace raksha::ir {

// Base class for attributes associated with IR elements.
class AttributeBase : public RefCounted<AttributeBase> {
 public:
  enum class Kind { kInt64, kString };
  AttributeBase(Kind kind) : kind_(kind) {}
  virtual ~AttributeBase() {}

  virtual Kind kind() const { return kind_; }
  virtual std::string ToString() const = 0;

 private:
  Kind kind_;
};

class Attribute {
 public:
  template <typename T, typename... Args,
            std::enable_if_t<std::is_convertible<T*, AttributeBase*>::value,
                             bool> = true>
  static Attribute Create(Args&&... a) {
    return T::Create(std::forward<Args>(a)...);
  }

  // Use default copy, move, and assignments.
  Attribute(const Attribute&) = default;
  Attribute(Attribute&&) = default;
  Attribute& operator=(const Attribute&) = default;
  Attribute& operator=(Attribute&&) = default;

  // Returns the attribute kind.
  AttributeBase::Kind kind() const { return value_->kind(); }

  // Returns a string representation of the attribute.
  std::string ToString() const { return value_->ToString(); }

 private:
  // Allows an `Attribute` instance  to be constructed from any intrusive_ptr
  // that is convertible to `intrusive_ptr<const AttributeBase*>`.
  template <class T,
            std::enable_if_t<std::is_convertible<T*, AttributeBase*>::value,
                             bool> = true>
  Attribute(const intrusive_ptr<const T>& value) : value_(value) {}

  intrusive_ptr<const AttributeBase> value_;
};

using NamedAttributeMap = absl::flat_hash_map<std::string, Attribute>;

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTES_ATTRIBUTE_H_
