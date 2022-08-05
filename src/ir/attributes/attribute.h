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
  enum class Kind { kInt64, kString, kDouble };
  AttributeBase(Kind kind) : kind_(kind) {}
  virtual ~AttributeBase() {}

  Kind kind() const { return kind_; }

  virtual bool IsEqual(const AttributeBase& other) const = 0;
  virtual std::string ToString() const = 0;

 private:
  Kind kind_;
};

class Attribute {
 public:
  // Factory method to create attributes of the appropriate kind. This
  // forwards the arguments to the corresponding T::create method to
  // create the correct attribute.
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

  // If this is of type `T` as identified by the `kind`, this method returns a
  // non-null value to the underlying attribute. Otherwise, returns nullptr.
  template <typename T>
  intrusive_ptr<const T> GetIf() const {
    return (T::kAttributeKind != value_->kind())
               ? nullptr
               : static_cast<const T*>(value_.get());
  }

  // Returns a string representation of the attribute.
  std::string ToString() const { return value_->ToString(); }

  friend bool operator==(const Attribute& lhs, const Attribute& rhs);

 private:
  // Private constructor that allows us to construct an attribute from an
  // intrusive_ptr to any derived type of `AttributeBase`.
  template <class T,
            std::enable_if_t<std::is_convertible<T*, AttributeBase*>::value,
                             bool> = true>
  Attribute(intrusive_ptr<const T> value) : value_(std::move(value)) {
    CHECK(value_ != nullptr);
  }

  intrusive_ptr<const AttributeBase> value_;
};

// TODO(#336): This will simply become a pointer comparison when we canonicalize
// the attributes.
inline bool operator==(const Attribute& lhs, const Attribute& rhs) {
  CHECK(lhs.value_ != nullptr && rhs.value_ != nullptr);
  if (lhs.value_ == rhs.value_) return true;
  return lhs.value_->IsEqual(*rhs.value_);
}

using NamedAttributeMap = absl::flat_hash_map<std::string, Attribute>;

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTES_ATTRIBUTE_H_
