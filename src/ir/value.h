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
#ifndef SRC_IR_VALUE_H_
#define SRC_IR_VALUE_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_format.h"
#include "src/ir/ssa_names.h"
#include "src/ir/storage.h"
#include "src/ir/types/type.h"

namespace raksha::ir {

class Block;
class Operation;
class Operator;
class Value;

namespace value {

// TODO: validate fields.

template <typename T>
class NamedValue {
 public:
  NamedValue(const T& element, absl::string_view name)
      : element_(&element), name_(name) {}

  NamedValue(const NamedValue&) = default;
  NamedValue& operator=(const NamedValue&) = default;
  NamedValue(NamedValue&&) = default;
  NamedValue& operator=(NamedValue&&) = default;

  const T& element() const { return *element_; }
  absl::string_view name() const { return name_; }
  std::string ToString(SsaNames& ssa_names) const {
    return absl::StrFormat("%%%d.%s", ssa_names.GetOrCreateID(*element_),
                           name_);
  }

 protected:
  // A helper that checks whether two `NamedValue`s are equal. We don't use
  // `operator==` because this is not a base class and we don't want to deal
  // with weird shadowing effects.
  template <typename U>
  static bool NamedValuesEq(const NamedValue<U>& obj1,
                            const NamedValue<U>& obj2) {
    return (obj1.element_ == obj2.element_) && (obj1.name_ == obj2.name_);
  }

 private:
  const T* element_;
  std::string name_;
};

// Indicates the argument to a block.
class BlockArgument : public NamedValue<Block> {
 public:
  using NamedValue<Block>::NamedValue;
  const Block& block() const { return element(); }
  bool operator==(const Value& other) const;
};

// Indicates the result of an operation.
class OperationResult : public NamedValue<Operation> {
 public:
  using NamedValue<Operation>::NamedValue;
  const Operation& operation() const { return element(); }
  bool operator==(const Value& other) const;
};

// Indicates the value in a storage.
class StoredValue {
 public:
  StoredValue(const Storage& storage) : storage_(&storage) {}

  StoredValue(const StoredValue&) = default;
  StoredValue& operator=(const StoredValue&) = default;

  const Storage& storage() const { return *storage_; }

  std::string ToString(const SsaNames& ssa_names) const {
    return storage_->ToString();
  }

  bool operator==(const Value& other) const;

 private:
  const Storage* storage_;
};

// A special value to indicate that it can be anything.
class Any {
 public:
  std::string ToString(const SsaNames& ssa_names) const { return "<<ANY>>"; }

  bool operator==(const Value& other) const;
};

}  // namespace value

// A class that represents a data value.
class Value {
 public:
  Value(value::BlockArgument arg) : value_(std::move(arg)) {}
  Value(value::OperationResult arg) : value_(std::move(arg)) {}
  Value(value::Any arg) : value_(std::move(arg)) {}
  Value(value::StoredValue arg) : value_(std::move(arg)) {}

  std::string ToString(SsaNames& ssa_names) const {
    return std::visit(
        [&ssa_names](const auto& variant) {
          return variant.ToString(ssa_names);
        },
        value_);
  }

  // A downcast operation. Just delegates directly to std::get on variants.
  template <class T>
  const T& As() const {
    return std::get<T>(value_);
  }

  // A dynamic downcast operation. Just delegates directly to std::get_if on
  // variants.
  template <class T>
  const T* If() const {
    return std::get_if<T>(&value_);
  }

  // Return whether the two `Value`s are identical representations. If they
  // are different, this does not necessarily mean that they do not
  // *evaluate* to the same value.
  bool operator==(const Value& other) const {
    // If the two `Value`s do not hold the same variant kind, they are not
    // equal.
    if (value_.index() != other.value_.index()) {
      return false;
    }
    return std::visit([&](const auto& val) { return val == other; }, value_);
  }

 private:
  std::variant<value::BlockArgument, value::OperationResult, value::StoredValue,
               value::Any>
      value_;
};

using ValueList = std::vector<Value>;
using NamedValueMap = absl::flat_hash_map<std::string, Value>;
using NamedValueListMap = absl::flat_hash_map<std::string, ValueList>;

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_H_
