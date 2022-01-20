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
#ifndef SRC_IR_OPERATION_H_
#define SRC_IR_OPERATION_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/ir/operator.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

// An Operation represents a unit of execution.
class Operation {
 public:
  Operation(const Block* parent, const Operator& op, NamedValueListMap inputs)
      : parent_(parent), op_(&op), inputs_(std::move(inputs)) {}

  // Disable copy (and move) semantics.
  Operation(const Operation&) = delete;
  Operation& operator=(const Operation&) = delete;

  const Operator& op() const { return *op_; }
  const Block* parent() const { return parent_; }
  const NamedValueListMap& inputs() const { return inputs_; }

 private:
  // The parent block if any to which this operation belongs to.
  const Block* parent_;
  // The operator being executed.
  const Operator* op_;
  // The inputs to the operator.
  NamedValueListMap inputs_;
};

}  // namespace raksha::ir


#endif  // SRC_IR_OPERATION_H_
