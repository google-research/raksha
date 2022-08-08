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
#ifndef SRC_IR_ATTRIBUTES_DOUBLE_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTES_DOUBLE_ATTRIBUTE_H_

#include "absl/strings/str_format.h"
#include "src/common/utils/intrusive_ptr.h"
#include "src/ir/attributes/attribute.h"

namespace raksha::ir {

class DoubleAttribute;
bool operator==(const DoubleAttribute& lhs, const DoubleAttribute& rhs);

class DoubleAttribute : public AttributeBase {
 public:
  static constexpr Kind kAttributeKind = Kind::kDouble;

  static intrusive_ptr<const DoubleAttribute> Create(double value) {
    return intrusive_ptr<const DoubleAttribute>(new DoubleAttribute(value));
  }

  double value() const { return value_; }

  bool IsEqual(const AttributeBase& other) const override {
    if (this->kind() != other.kind()) return false;
    return *this == static_cast<const DoubleAttribute&>(other);
  }

  std::string ToString() const override {
    return absl::StrFormat("%gl", value_);
  }

 private:
  DoubleAttribute(double value)
      : AttributeBase(kAttributeKind), value_(value) {}

  double value_;
};

inline bool operator==(const DoubleAttribute& lhs, const DoubleAttribute& rhs) {
  return lhs.value() == rhs.value();
}

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTES_DOUBLE_ATTRIBUTE_H_
