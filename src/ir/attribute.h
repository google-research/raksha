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
#ifndef SRC_IR_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTE_H_

#include "absl/strings/string_view.h"
#include "src/utils/intrusive_ptr.h"

namespace raksha::ir {

// Base class for attributes associated with IR elements.
class AttributeBase : public RefCounted<AttributeBase> {
 public:
  virtual ~AttributeBase() {}

 private:
  // TODO: Add kind.
};

class IntAttribute : public AttributeBase {
 public:
  IntAttribute(int value) : value_(value) {}
  int value() const { return value_; }

 private:
  int value_;
};

class StringAttribute : public AttributeBase {
 public:
  StringAttribute(absl::string_view value) : value_(value) {}
  absl::string_view value() const { return value_; }

 private:
  std::string value_;
};

using Attribute = intrusive_ptr<const AttributeBase>;
using NamedAttributeMap = absl::flat_hash_map<std::string, Attribute>;

class AttributeFactory {
 public:
  static Attribute MakeStringAttribute(absl::string_view value) {
    return Attribute(new StringAttribute(value));
  }
  static Attribute MakeIntAttribute(int value) {
    return Attribute(new IntAttribute(value));
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTE_H_
