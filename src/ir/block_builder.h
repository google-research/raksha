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
#ifndef SRC_IR_BLOCK_BUILDER_H_
#define SRC_IR_BLOCK_BUILDER_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/ir/module.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

class BlockBuilder {
 public:
  BlockBuilder() : block_(std::make_unique<Block>()) {}

  BlockBuilder& AddInput(std::string name, types::Type type) {
    block_->inputs_.AddDecl(name, std::move(type));
    return *this;
  }

  BlockBuilder& AddOutput(std::string name, types::Type type) {
    block_->outputs_.AddDecl(name, std::move(type));
    return *this;
  }

  const Operation& AddOperation(const Operator& op,
                                NamedAttributeMap attributes,
                                NamedValueMap inputs) {
    std::unique_ptr<Operation> operation(new Operation(
        block_.get(), op, std::move(attributes), std::move(inputs)));
    const Operation* result = operation.get();
    block_->operations_.push_back(std::move(operation));
    return *result;
  }

  BlockBuilder& AddImplementation(
      std::function<void(BlockBuilder&, Block&)> builder) {
    builder(*this, *block_);
    return *this;
  }

  BlockBuilder& AddResult(absl::string_view name, Value output) {
    CHECK(block_->outputs_.FindDecl(name) != nullptr);
    block_->results_.insert({std::string(name), output});
    return *this;
  }

  std::unique_ptr<Block> build() { return std::move(block_); }

 private:
  std::unique_ptr<Block> block_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_BLOCK_BUILDER_H_
