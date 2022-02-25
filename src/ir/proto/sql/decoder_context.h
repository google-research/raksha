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

#ifndef SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_
#define SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_

#include <memory>

#include "absl/container/flat_hash_map.h"
#include "src/ir/ir_context.h"
#include "src/ir/value.h"
#include "src/ir/proto/sql/sql_ir.pb.h"

namespace raksha::ir::proto::sql {

// Context specifically for the SQL proto decoding. This keeps track of state
// that is not required when building the IR in general, and thus would be
// inappropriate to put in IRContext.
class DecoderContext {
 public:
  DecoderContext(IRContext &ir_context) : ir_context_(ir_context) {}

  // Register a value as associated with a particular id. This stores that
  // value and provides a reference to its canonical value.
  const Value &RegisterValue(uint64_t id, Value value) {
    auto insert_result =
        id_to_value.insert({id, std::make_unique<Value>(std::move(value))});
    CHECK(insert_result.second)
      << "id_to_value map has more than one value associated with the id "
      << insert_result.first->first << ".";
    return *insert_result.first->second;
  }

  // Get a reference to the value associated with a particular id. Assumes that
  // the ID is present; it will error if it is not.
  const Value &GetValue(uint64_t id) const {
    // We could use `at`, which would be more concise and would also fail if the
    // id is not present in the map, but it will not provide a nice error
    // message. Use `find` and `CHECK` to get a better error message.
    auto find_result = id_to_value.find(id);
    CHECK(find_result != id_to_value.end())
      << "Could not find a value with id " << id << ".";
    return *find_result->second;
  }

  IRContext &ir_context() { return ir_context_; }

 private:
  IRContext &ir_context_;
  absl::flat_hash_map<uint64_t, std::unique_ptr<Value>> id_to_value;
};

}  // namespace raksha::ir::proto::sql

#endif  // SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_
