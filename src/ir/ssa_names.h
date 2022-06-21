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
#include "absl/status/statusor.h"

namespace raksha::ir {

class Operation;
class Block;
class Module;
class Value;

class SsaNames {
 public:
  using ID = std::string;
  SsaNames() {}
  // Disable copy (and move) semantics.
  // SsaNames(const SsaNames &) = delete;
  // SsaNames &operator=(const SsaNames &) = delete;

  ID GetOrCreateID(const Operation *operation) {
    return operation_ids_.GetOrCreateID(operation);
  }

  ID GetOrCreateID(const Operation *operation, ID id) {
    return operation_ids_.GetOrCreateID(operation, id);
  }
  ID GetOrCreateID(const Block *block) {
    return block_ids_.GetOrCreateID(block);
  }

  ID GetOrCreateID(const Block *block, ID id) {
    return block_ids_.GetOrCreateID(block, id);
  }

  bool findID(const Block *block) { return block_ids_.findID(block); }

  ID GetOrCreateID(const Module *module) {
    return module_ids_.GetOrCreateID(module);
  }
  ID GetOrCreateID(const Module *module, ID id) {
    return module_ids_.GetOrCreateID(module, id);
  }
  bool findID(const Module *module) { return module_ids_.findID(module); }

  ID GetOrCreateID(const Value *value) {
    return value_ids_.GetOrCreateID(value);
  }

  ID GetOrCreateID(const Value *value, ID id) {
    return value_ids_.GetOrCreateID(value, id);
  }
  // void UpdateID(const Value &value) {
  //   // check that v is not already there in the map
  //   // and update
  //   if (!value_ids_.findID(&value)) {
  //     value_ids_.GetOrCreateID(&value);
  //   }
  //   // return find_value->second;
  //   // return absl::OkStatus();
  // }

  // absl::Status<ID> UpdateID(Value v, ID id) {
  //   // check that v is not already there in the map
  //   // and update
  //   auto find_value = value_ids_.find(v);
  //   if (find_value == value_map_.end()) {
  //     return value_ids_.GetOrCreateID(v, id);
  //   }
  //   return find_value->second;
  //   // return absl::OkStatus();
  // }

 private:
  template <class T>
  class IDManager {
   public:
    ID GetOrCreateID(const T *entity) {
      // Note that if `entity` is already in the map, the `insert` call below
      // will return the existing entity. Otherwise, a new entity is added.
      auto insert_result = item_ids_.insert(
          {entity, absl::StrCat(absl::StrCat(item_ids_.size() + 1))});
      return insert_result.first->second;
    }
    ID GetOrCreateID(const T *entity, ID id) {
      // Note that if `entity` is already in the map, the `insert` call below
      // will return the existing entity. Otherwise, a new entity is added.
      auto insert_result = item_ids_.insert({entity, absl::StrCat(id)});
      return insert_result.first->second;
    }

    bool findID(const T *entity) {
      return item_ids_.find(entity) != item_ids_.end();
    }

   private:
    // absl::flat_hash_map<T, ID> item_ids_;
    absl::flat_hash_map<const T *, ID> item_ids_;
  };

  // IDManager<Operation> operation_ids_;
  // IDManager<const Block *> block_ids_;
  // IDManager<const Module *> module_ids_;
  IDManager<Block> block_ids_;
  IDManager<Module> module_ids_;
  IDManager<Value> value_ids_;
  IDManager<Operation> operation_ids_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_CONTEXT_H_
