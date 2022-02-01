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
#include "absl/strings/str_join.h"
#include "src/ir/attribute.h"
#include "src/ir/operator.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

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

  std::string ToString() const {
    constexpr absl::string_view kOperationFormat = "%s [%s](%s)";
    std::string attributes_string;
    for (const auto& [name, attribute] : attributes_) {
      absl::StrAppendFormat(&attributes_string, "%s: %s", name,
                            attribute->ToString());
    }
    std::string inputs_string;
    for (const auto& [name, input] : inputs_) {
      absl::StrAppendFormat(&inputs_string, "%s: %s", name, input.ToString());
    }
    return absl::StrFormat(kOperationFormat, op_->name(), attributes_string,
                           inputs_string);
  }

 private:
  // The parent block if any to which this operation belongs to.
  const Block* parent_;
  // The operator being executed.
  const Operator* op_;
  // The attributes of the operation.
  NamedAttributeMap attributes_;
  // The inputs of the operation.
  NamedValueMap inputs_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_OPERATION_H_
