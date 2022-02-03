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
#include "src/ir/block.h"
#include "src/ir/data_decl.h"
#include "src/ir/operation.h"
#include "src/ir/operator.h"
#include "src/ir/types/type.h"
#include "src/ir/value.h"

namespace raksha::ir {

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