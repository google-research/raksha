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
#ifndef SRC_IR_IR_TO_PROTO_H_
#define SRC_IR_IR_TO_PROTO_H_

#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/proto/raksha_ir.pb.h"
#include "src/ir/value.h"

namespace raksha::ir {

class IRToProto {
 public:
  using ID = uint64_t;
  static proto::IrTranslationUnit Convert(const IRContext& context,
                                          const Module& root) {
    // Construct a context to hold state for the traversal.
    ID last_used_id = 0;
    IRToProto state(last_used_id);

    proto::IrTranslationUnit result;
    const auto& ops = context.Operators();
    for (const auto& op : ops) {
      proto::Operator* op_proto = result.add_operators();
      op_proto->set_name(std::string(op.first));
    }

    (*result.mutable_frontend()) = "Raksha.IRToProto";
    (*result.mutable_top_level_module()) = state.ModuleToProto(root);

    return result;
  }

 private:
  IRToProto(ID& id)
      : last_used_id_{id} {}

  // Disable copy (and move) semantics.
  IRToProto(const IRToProto&) = delete;
  IRToProto& operator=(const IRToProto&) = delete;

  virtual ~IRToProto() {}

  proto::Module ModuleToProto(const Module&);
  proto::Block BlockToProto(const Block&);
  proto::Operation OperationToProto(const Operation&);
  proto::Value ValueToProto(const Value&);
  proto::AnyValue AnyToProto(const value::Any&);
  proto::BlockArgumentValue BlockArgumentToProto(const value::BlockArgument&);
  proto::OperationResultValue OperationResultToProto(
      const value::OperationResult&);
  proto::AttributePayload AttributeToProto(const Attribute&);

  template <typename T>
  const ID GetID(const T& thing_to_get_value_for) {
    // Ensure that each ptr has a single unique ID.
    const void* ptr = &thing_to_get_value_for;
    auto insert_result = used_ids_.insert({ptr, last_used_id_ + 1});
    if (!insert_result.second) { return insert_result.first->second; }
    return ++last_used_id_;
  }

  absl::flat_hash_map<const void*, ID> used_ids_;
  // This is a reference so that a two translations can more easily avoid having
  // overlapping IDs.
  ID& last_used_id_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_TO_PROTO_H_
