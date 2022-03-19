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
#ifndef SRC_IR_ATTRIBUTES_STRING_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTES_STRING_ATTRIBUTE_H_

#include "absl/strings/string_view.h"
#include "src/ir/attributes/attribute.h"

namespace raksha::ir {
class StringAttribute : public AttributeBase {
 public:
  static Attribute Create(absl::string_view value) {
    return Attribute(new StringAttribute(value));
  }

  absl::string_view value() const { return value_; }
  std::string ToString() const override { return value_; }

 private:
  StringAttribute(absl::string_view value)
      : AttributeBase(Kind::kString), value_(value) {}

  std::string value_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTES_STRING_ATTRIBUTE_H_
