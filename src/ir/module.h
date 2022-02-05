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
#ifndef SRC_IR_MODULE_H_
#define SRC_IR_MODULE_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/ir/attribute.h"
#include "src/ir/data_decl.h"
#include "src/ir/operator.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

class Module;
class Block;

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

  std::string ToString(SSANames& ssa_names) const;

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

// A collection of operations.
class Block {
 public:
  // The type for a collection of `Operation` instances.
  using OperationList = std::vector<std::unique_ptr<Operation>>;

  Block() = default;

  // Disable copy (and move) semantics.
  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

  const OperationList& operations() const { return operations_; }
  const DataDeclCollection& inputs() const { return inputs_; }
  const DataDeclCollection& outputs() const { return outputs_; }
  const NamedValueMap& results() const { return results_; }
  const Module* module() const { return module_; }

  friend class BlockBuilder;

 private:
  // Module to which this belongs to.
  const Module* module_;
  // The inputs to this block.
  DataDeclCollection inputs_;
  // Outputs from this block.
  DataDeclCollection outputs_;
  // The list of operations that belong to this block. This can be empty.
  OperationList operations_;
  // Maps the outputs of the operations in the list of operations in this
  // block to the corresponding name. Note that a result can have more than
  // one value which is used to represent non-determinism.
  NamedValueMap results_;
};

// A class that contains a collection of blocks.
class Module {
 public:
  Module(const Module&) = delete;
  Module& operator=(const Module&) = delete;
  Module(Module&&) = default;
  Module& operator=(Module&&) = default;

  // Adds a block to the module and returns a pointer to it.
  const Block* AddBlock(std::unique_ptr<Block> block) {
    const Block* result = block.get();
    blocks_.push_back(std::move(block));
    return result;
  }

 private:
  using BlockListType = std::vector<std::unique_ptr<Block>>;
  BlockListType blocks_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_MODULE_H_
