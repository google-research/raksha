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

#include "src/ir/attribute.h"
#include "src/ir/operator.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

class Module;

// An Operation represents a unit of execution.
class Operation {
 public:
  Operation(const Block* parent, const Operator& op,
            NamedAttributeMap attributes, NamedValueMap inputs)
      : parent_(parent),
        op_(&op),
        attributes_(std::move(attributes)),
        inputs_(std::move(inputs)) {}

  // Disable copy (and move) semantics.
  Operation(const Operation&) = delete;
  Operation& operator=(const Operation&) = delete;

  const Operator& op() const { return *op_; }
  const Block* parent() const { return parent_; }
  const NamedValueMap& inputs() const { return inputs_; }
  const NamedAttributeMap& attributes() const { return attributes_; }

  // Declaring the destructor explicitly and implementing it in the cc file to
  // avoid circular include dependency between Module and Operation.
  ~Operation();

  std::string ToString() const;

 private:
  // The parent block if any to which this operation belongs to.
  const Block* parent_;
  // The operator being executed.
  const Operator* op_;
  // The attributes of the operation.
  NamedAttributeMap attributes_;
  // The inputs of the operation.
  NamedValueMap inputs_;
  // If the operation can be broken down into other operations, it is
  // specified in the optional module. If `module_` is nullptr, then this is a
  // basic operator like `+`, `-`, etc., which cannot be broken down further.
  std::unique_ptr<Module> module_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_OPERATION_H_