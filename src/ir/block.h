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
#ifndef SRC_IR_BLOCK_H_
#define SRC_IR_BLOCK_H_

#include "src/ir/data_decl.h"
#include "src/ir/operation.h"
#include "src/ir/value.h"

namespace raksha::ir {

class Module;

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

}  // namespace raksha::ir

#endif  // SRC_IR_BLOCK_H_