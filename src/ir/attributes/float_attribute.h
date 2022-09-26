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
#ifndef SRC_IR_ATTRIBUTES_FLOAT_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTES_FLOAT_ATTRIBUTE_H_

#include <memory>

#include "absl/strings/str_format.h"
#include "src/ir/attributes/attribute.h"

namespace raksha::ir {

class FloatAttribute;
bool operator==(const FloatAttribute& lhs, const FloatAttribute& rhs);

class FloatAttribute : public AttributeBase {
 public:
  static constexpr Kind kAttributeKind = Kind::kFloat;

  static std::shared_ptr<const FloatAttribute> Create(double value) {
    return std::shared_ptr<const FloatAttribute>(new FloatAttribute(value));
  }

  double value() const { return value_; }

  bool IsEqual(const AttributeBase& other) const override {
    if (this->kind() != other.kind()) return false;
    return *this == static_cast<const FloatAttribute&>(other);
  }

  std::string ToString() const override {
    return absl::StrFormat("%lgl", value_);
  }

 private:
  FloatAttribute(double value) : AttributeBase(kAttributeKind), value_(value) {}

  double value_;
};

inline bool operator==(const FloatAttribute& lhs, const FloatAttribute& rhs) {
  return lhs.value() == rhs.value();
}

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTES_FLOAT_ATTRIBUTE_H_
