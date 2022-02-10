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
//-----------------------------------------------------------------------------
#ifndef SRC_IR_SSA_NAMES_H_
#define SRC_IR_SSA_NAMES_H_

#include "absl/container/flat_hash_map.h"

namespace raksha::ir {

class Block;
class Operation;

class SsaNames {
 public:
  using ID = int;
  SsaNames() {}
  // Disable copy (and move) semantics.
  SsaNames(const SsaNames &) = delete;
  SsaNames &operator=(const SsaNames &) = delete;

  ID GetOrCreateID(const Operation &operation) {
    return operation_ids_.GetOrCreateID(&operation);
  }

  ID GetOrCreateID(const Block &block) {
    return block_ids_.GetOrCreateID(&block);
  }

 private:
  template <class T>
  class IDManager {
   public:
    ID GetOrCreateID(const T *entity) {
      // Note that if `entity` is already in the map, the `insert` call below
      // will return the existing entity. Otherwise, a new entity is added.
      auto insert_result = item_ids_.insert({entity, item_ids_.size()});
      return insert_result.first->second;
    }

   private:
    absl::flat_hash_map<const T *, ID> item_ids_;
  };

  IDManager<Block> block_ids_;
  IDManager<Operation> operation_ids_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_CONTEXT_H_
