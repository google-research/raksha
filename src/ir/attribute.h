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
#ifndef SRC_IR_ATTRIBUTE_H_
#define SRC_IR_ATTRIBUTE_H_

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/utils/intrusive_ptr.h"

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

using Attribute = intrusive_ptr<const AttributeBase>;

class Int64Attribute : public AttributeBase {
 public:
  static Attribute Create(int value) {
    return Attribute(new Int64Attribute(value));
  }

  int value() const { return value_; }

  std::string ToString() const override {
    return absl::StrFormat("%d", value_);
  }

 private:
  Int64Attribute(int64_t value) : AttributeBase(Kind::kInt64), value_(value) {}

  int64_t value_;
};

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

using NamedAttributeMap = absl::flat_hash_map<std::string, Attribute>;

}  // namespace raksha::ir

#endif  // SRC_IR_ATTRIBUTE_H_
