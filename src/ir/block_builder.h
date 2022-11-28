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
#ifndef SRC_IR_BLOCK_BUILDER_H_
#define SRC_IR_BLOCK_BUILDER_H_

#include <type_traits>
#include <vector>

#include "src/ir/module.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

class BlockBuilder {
 public:
  BlockBuilder() : block_(std::make_unique<Block>()) {}

  BlockBuilder& AddInput(absl::string_view name, types::Type type) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    block_->inputs_.AddDecl(name, std::move(type));
    return *this;
  }

  BlockBuilder& AddOutput(absl::string_view name, types::Type type) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    block_->outputs_.AddDecl(name, std::move(type));
    return *this;
  }

  BlockBuilder& AddResult(absl::string_view name, Value output) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    CHECK(block_->outputs_.FindDecl(name) != nullptr)
        << "Output '" << name << "' is not declared in the block.";
    block_->results_.insert({std::string(name), output});
    return *this;
  }

  // Constructs and adds an operation to the block and returns the operation.
  const Operation& AddOperation(const Operator& op,
                                NamedAttributeMap attributes,
                                ValueList inputs) {
    return AddOperation(std::make_unique<Operation>(op, std::move(attributes),
                                                    std::move(inputs)));
  }

  // Sets parent of operation and adds an operation to the block and returns
  // the operation.
  const Operation& AddOperation(std::unique_ptr<Operation> op) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    CHECK(op != nullptr) << "`AddOperation` received a nullptr.";
    CHECK(op->parent() == nullptr)
        << "`AddOperation` received an operation with its Parent pointer "
           "already defined. Cannot set parent again!";
    op->set_parent(block_.get());
    block_->operations_.push_back(std::move(op));
    return *block_->operations_.back();
  }

  // Allows an operation to be added to the block using a `T::Create` method as
  // long as the following hold:
  //   - `T` is a derived class of `Operation`.
  //   - `T::create` takes a pointer to parent block as a first argument.
  // This method also checks that T::Create does not return a nullptr and
  // sets the parent block correctly.
  template <typename T, typename... Args,
            std::enable_if_t<std::is_base_of<Operation, T>::value, bool> = true>
  const Operation& AddOperation(Args&&... a) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    std::unique_ptr<Operation> result =
        T::Create(block_.get(), std::forward<Args>(a)...);
    CHECK(result != nullptr)
        << "`T::Create` returned a nullptr when adding an operation.";
    CHECK(result->parent() == block_.get())
        << "`T::Create` did not set the correct parent block.";
    block_->operations_.push_back(std::move(result));
    return *block_->operations_.back();
  }

  // Provides the ability to make multiple changes to the block through a
  // lambda. Here is an example:
  //
  //   BlockBuilder()
  //     .AddImplementation([](BlockBuilder& builder, Block& block) {
  //        auto op = builder.AddOperation(...);
  //        builder.AddResult(Value(OperationResult(op, "x")));
  //     })
  //     .build();
  //
  BlockBuilder& AddImplementation(
      std::function<void(BlockBuilder&, Block&)> builder) {
    CHECK(!IsBuilt())
        << "Attempt to use a `BlockBuilder` that has already been built.";
    builder(*this, *block_);
    return *this;
  }

  std::unique_ptr<Block> build() {
    // After we call `build` on a `BlockBuilder`, the `block_` field is moved
    // from. This causes the pointer within the `block_` field to be stolen
    // by `std::unique_ptr`'s move constructor, leaving a `nullptr` in its
    // place. The following check makes sure that we don't attempt to call
    // `build` twice on the same `BlockBuilder` by checking that `block_` is
    // not `nullptr`.
    CHECK(block_) << "Attempt to build a `BlockBuilder` that has already been"
                     " built.";
    return std::move(block_);
  }

  // A way to get non-owning access to the block (for reference by operations)
  // while the block is still under construction.
  Block* GetBlockPtr() {
    // The following check makes sure that we don't attempt to call
    // `unsafe_get_block_ptr` on an empty `BlockBuilder` by checking that
    // `block_` is not `nullptr`.
    CHECK(block_) << "Attempt to get block pointer from a `BlockBuilder` that "
                     "has already been"
                     " built.";
    return block_.get();
  }

  bool IsBuilt() const { return block_.get() == nullptr; }

 private:
  std::unique_ptr<Block> block_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_BLOCK_BUILDER_H_
