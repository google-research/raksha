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

#include "absl/container/flat_hash_map.h"
#include "src/ir/module.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

class BlockBuilder {
 public:
  BlockBuilder() : block_(std::make_unique<Block>()) {}

  BlockBuilder& AddInput(absl::string_view name, types::Type type) {
    block_->inputs_.AddDecl(name, std::move(type));
    return *this;
  }

  BlockBuilder& AddOutput(absl::string_view name, types::Type type) {
    block_->outputs_.AddDecl(name, std::move(type));
    return *this;
  }

  BlockBuilder& AddResult(absl::string_view name, Value output) {
    CHECK(block_->outputs_.FindDecl(name) != nullptr)
        << "Output '" << name << "' is not declared in the block.";
    block_->results_.insert({std::string(name), output});
    return *this;
  }

  // Adds an operation to the block and returns the operation.
  const Operation& AddOperation(const Operator& op,
                                NamedAttributeMap attributes, ValueList inputs,
                                std::unique_ptr<Module> impl_module = nullptr) {
    block_->operations_.push_back(
        std::make_unique<Operation>(block_.get(), op, std::move(attributes),
                                    std::move(inputs), std::move(impl_module)));
    return *block_->operations_.back();
  }

  // Sets parent of operation and adds an operation to the block and returns
  // the operation.
  const Operation& AddOperation(std::unique_ptr<Operation> op) {
    CHECK(op->parent() == nullptr)
        << "Parent pointer already defined. Cannot set parent again!";
    op->set_parent(block_.get());
    block_->operations_.push_back(std::move(op));
    return *block_->operations_.back();
  }

  // Allows an operation to be added to the block using a `T::Create` method as
  // long as the following hold:
  //   - `T` is a derived class of `Operation`.
  //   - `T::create` takes a pointer to parent block as a first arguemnt.
  // This method also checks that T::Create does not return a nullptr and
  // sets the parent block correctly.
  template <typename T, typename... Args,
            std::enable_if_t<std::is_base_of<Operation, T>::value, bool> = true>
  const Operation& AddOperation(Args&&... a) {
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

 private:
  std::unique_ptr<Block> block_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_BLOCK_BUILDER_H_
