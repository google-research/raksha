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

#include "src/common/containers/hash_map.h"
#include "src/ir/value.h"

namespace raksha::ir {

class Operation;
class Block;
class Module;
class Value;

class SsaNames {
 public:
  using ID = std::string;
  SsaNames() : block_ids_("b"), module_ids_("m"), value_ids_("%") {}

  // Disable copy semantics.
  SsaNames(const SsaNames &) = delete;
  SsaNames &operator=(const SsaNames &) = delete;
  SsaNames(SsaNames &&) = default;

  ID GetOrCreateID(const Block &block) {
    return block_ids_.GetOrCreateID(&block);
  }

  ID AddID(const Block &block, ID id) {
    CHECK(block_ids_.FindID(&block))
        << "Trying to update new ID " << id << " to an existing block map";
    return block_ids_.UpdateID(&block, id);
  }

  ID GetOrCreateID(const Module &module) {
    return module_ids_.GetOrCreateID(&module);
  }
  ID AddID(const Module &module, ID id) {
    CHECK(module_ids_.FindID(&module))
        << "Trying to update new ID " << id << " to an existing module map";
    return module_ids_.UpdateID(&module, id);
  }

  ID GetOrCreateID(Value value) { return value_ids_.GetOrCreateID(value); }

  ID AddID(Value value, ID id) {
    CHECK(value_ids_.FindID(value))
        << "Trying to update new ID " << id << " to an existing value map";
    return value_ids_.UpdateID(value, id);
  }

 private:
  template <typename T>
  class IDManager {
   public:
    IDManager(std::string_view prefix) : prefix_(prefix) {}

    ID GetOrCreateID(T entity) {
      // Note that if `entity` is already in the map, the `insert` call below
      // will return the existing entity. Otherwise, a new entity is added.
      auto insert_result =
          item_ids_.insert({entity, absl::StrCat(prefix_, item_ids_.size())});
      return insert_result.first->second;
    }

    ID UpdateID(T entity, ID id) {
      // Note that if `entity` is already in the map, we replace the id
      // and return the existing new id.
      auto insert_result = item_ids_.insert_or_assign(entity, id);
      return insert_result.first->second;
    }

    bool FindID(T entity) {
      auto result = item_ids_.find(entity);
      return result == item_ids_.end();
    }

   private:
    std::string prefix_;
    common::containers::HashMap<T, ID> item_ids_;
  };

  IDManager<const Block *> block_ids_;
  IDManager<const Module *> module_ids_;
  IDManager<Value> value_ids_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_CONTEXT_H_
