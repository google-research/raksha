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

 private:
  const T* element_;
  std::string name_;
};

class BlockArgument : NamedValue<Block> {
 public:
  using NamedValue<Block>::NamedValue;
  const Block& block() const { return element(); }
};

class BlockResult : NamedValue<Block> {
 public:
  using NamedValue<Block>::NamedValue;
  const Block& block() const { return element(); }
};

class OperationResult : NamedValue<Operation> {
 public:
  using NamedValue<Operation>::NamedValue;
  const Operation& operation() const { return element(); }
};

// A field within another value.
// Note: copying around `Field` values can be expensive.
class Field {
 public:
  Field(const Value& parent, std::string field_name)
      : parent_(std::make_unique<Value>(parent)),
        field_name_(std::move(field_name)) {}

  // Custom copy constructor.
  Field(const Field& other)
      : parent_(std::make_unique<Value>(*other.parent_)),
        field_name_(other.field_name_) {}

  // Custom assignment operator.
  Field& operator=(Field other) {
    using std::swap;
    swap(*this, other);
    return *this;
  }

  // Default move semantics
  Field(Field&&) = default;

 private:
  // We use `unique_ptr` to break the cycle involving variants of a `Value`.
  std::unique_ptr<Value> parent_;
  std::string field_name_;
};

// Indicates the value in a storage.
class StoredValue {
 public:
  StoredValue(const Storage& storage) : storage_(&storage) {}

  StoredValue(const StoredValue&) = default;
  StoredValue& operator=(const StoredValue&) = default;

  const Storage& storage() const { return *storage_; }

 private:
  const Storage* storage_;
};

// TODO:
class Constant {
  // int, string, etc.
};

// A special value to indicate that it can be anything.
class Any {};

}  // namespace value

// A class that represents a data value.
class Value {
 public:
  Value(value::BlockArgument arg) : value_(std::move(arg)) {}
  Value(value::BlockResult arg) : value_(std::move(arg)) {}
  Value(value::OperationResult arg) : value_(std::move(arg)) {}
  Value(value::Field arg) : value_(std::move(arg)) {}
  Value(value::Constant arg) : value_(std::move(arg)) {}
  Value(value::Any arg) : value_(std::move(arg)) {}
  Value(value::StoredValue arg) : value_(std::move(arg)) {}

 private:
  std::variant<value::BlockArgument, value::BlockResult, value::OperationResult,
               value::Field, value::StoredValue, value::Constant, value::Any>
      value_;
};

using ValueList = std::vector<Value>;
using NamedValueMap = absl::flat_hash_map<std::string, Value>;
using NamedValueListMap = absl::flat_hash_map<std::string, ValueList>;

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_H_
