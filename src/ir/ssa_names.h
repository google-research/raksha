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

class SSANames {
 public:
  using ID = int;
  SSANames() : next_block_id_(0), next_operation_id_(0) {}

  // Disable copy (and move) semantics.
  SSANames(const SSANames &) = delete;
  SSANames &operator=(const SSANames &) = delete;

  ID GetOrCreateID(const Operation &operation) {
    return GetOrCreateIDHelper(operation_ids_, next_operation_id_, &operation);
  }

  ID GetOrCreateID(const Block &block) {
    return GetOrCreateIDHelper(block_ids_, next_block_id_, &block);
  }

 private:
  template <typename T, typename U>
  static ID GetOrCreateIDHelper(T &ids_table, ID &next_id, U entity) {
    auto find_result = ids_table.find(entity);
    if (find_result == ids_table.end()) {
      ID result = next_id++;
      ids_table.insert({entity, result});
      return result;
    }
    return find_result->second;
  }
  ID next_block_id_;
  absl::flat_hash_map<const Block *, ID> block_ids_;
  ID next_operation_id_;
  absl::flat_hash_map<const Operation *, ID> operation_ids_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_CONTEXT_H_
