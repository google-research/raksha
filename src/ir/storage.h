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
#ifndef SRC_IR_STORAGE_H_
#define SRC_IR_STORAGE_H_

#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_format.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

// A class that represents a storage node.
class Storage {
 public:
  Storage(absl::string_view name, types::Type type)
      : name_(name), type_(std::move(type)) {}

  // Disable copy semantics.
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;
  Storage(Storage&&) = default;
  Storage& operator=(Storage&&) = default;
  ~Storage() = default;

  void AddInputValue(Value value) { input_values_.insert(value); }

  absl::string_view name() const { return name_; }
  const types::Type& type() const { return type_; }
  const absl::flat_hash_set<Value>& input_values() const {
    return input_values_;
  }

  template <typename Derived, typename Result>
  Result Accept(IRVisitor<Derived, Result, false>& visitor) {
    return visitor.Visit(*this);
  }

  template <typename Derived, typename Result>
  Result Accept(IRVisitor<Derived, Result, true>& visitor) const {
    return visitor.Visit(*this);
  }

  std::string ToString() const {
    // TODO(#335): When types have a ToString use that to print types.
    return absl::StrFormat("store:%s:type", name_);
  }

 private:
  std::string name_;
  types::Type type_;
  absl::flat_hash_set<Value> input_values_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_STORAGE_H_
