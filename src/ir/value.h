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

// A value that is an argument to a block.
class BlockArgument {
 public:
  BlockArgument(const Block& block, std::string name)
      : block_(&block), argument_name_(std::move(name)) {}

  BlockArgument(const BlockArgument&) = default;
  BlockArgument& operator=(const BlockArgument&) = default;

 private:
  const Block* block_;
  std::string argument_name_;
};

// A value that is the output of a block.
class BlockResult {
 public:
  BlockResult(const Block& block, std::string name)
      : block_(&block), result_name_(std::move(name)) {}

  BlockResult(const BlockResult&) = default;
  BlockResult& operator=(const BlockResult&) = default;

 private:
  const Block* block_;
  std::string result_name_;
};

// A value that is the result of an operation.
class OperationResult {
 public:
  OperationResult(const Operation& op, std::string name)
      : operation_(&op), operand_name_(name) {}

  OperationResult(const OperationResult&) = default;
  OperationResult& operator=(const OperationResult&) = default;
 private:
  const Operation* operation_;
  std::string operand_name_;
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
  std::unique_ptr<Value> parent_;
  std::string field_name_;
};

// Indicates the value in a storage.
class Store {
 public:
  Store(const Storage& storage) : storage_(&storage) {}

  Store(const Store&) = default;
  Store& operator=(const Store&) = default;

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
  Value(value::BlockArgument arg): value_(std::move(arg)) {}
  Value(value::BlockResult arg): value_(std::move(arg)) {}
  Value(value::OperationResult arg) : value_(std::move(arg)) {}
  Value(value::Field arg) : value_(std::move(arg)) {}
  Value(value::Constant arg) : value_(std::move(arg)) {}
  Value(value::Any arg) : value_(std::move(arg)) {}
  Value(value::Store arg) : value_(std::move(arg)) {}

 private:
  std::variant<value::BlockArgument, value::BlockResult, value::OperationResult,
               value::Field, value::Store, value::Constant, value::Any>
      value_;
};

using ValueList = std::vector<Value>;
using NamedValueMap = absl::flat_hash_map<std::string, Value>;
using NamedValueListMap = absl::flat_hash_map<std::string, ValueList>;
using DataDeclMap = absl::flat_hash_map<std::string, types::Type>;

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_H_
