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
#include "src/common/utils/ranges.h"
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
  bool operator==(const NamedValue<T>& other) const {
    return (element_ == other.element_) && (name_ == other.name_);
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
  using NamedValue<Block>::operator==;
};

// Indicates the result of an operation.
class OperationResult : public NamedValue<Operation> {
 public:
  using NamedValue<Operation>::NamedValue;
  const Operation& operation() const { return element(); }
  using NamedValue<Operation>::operator==;
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

  bool operator==(const StoredValue& other) const {
    return storage_ == other.storage_;
  }

 private:
  const Storage* storage_;
};

// A special value to indicate that it can be anything.
class Any {
 public:
  std::string ToString(const SsaNames& ssa_names) const { return "<<ANY>>"; }

  // Two `Any`s are always the same, as `Any` has no internal structure to
  // differ.
  bool operator==(const Any& other) const { return true; }
};

}  // namespace value

// A class that represents a data value.
class Value {
 public:
  using Variants = std::variant<value::BlockArgument, value::OperationResult,
                                value::StoredValue, value::Any>;

  explicit Value(Variants value) : value_(std::move(value)) {}

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
  bool operator==(const Value& other) const { return value_ == other.value_; }

 private:
  Variants value_;
};

using ValueList = std::vector<Value>;
using ValueRange = utils::iterator_range<ValueList::const_iterator>;
using NamedValueMap = absl::flat_hash_map<std::string, Value>;
using NamedValueListMap = absl::flat_hash_map<std::string, ValueList>;

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_H_
