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

#include "src/ir/proto_to_ir.h"

#include <memory>
#include <variant>
#include <optional>

#include "src/ir/attributes/double_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"

namespace raksha::ir {

std::unique_ptr<Module> ProtoToIR::PreVisit(const proto::Module& module_proto) {
  auto module = std::make_unique<Module>();
  for (const auto& block_it : module_proto.blocks()) {
    blocks_.insert({block_it.id(), BlockBuilder()});
    PreVisit(block_it.block());
  }
  return module;
}

void ProtoToIR::PreVisit(const proto::Block& block_proto) {
  for (const auto& operation_it : block_proto.operations()) {
    const auto& operation_proto = operation_it.operation();
    const auto& name = operation_proto.operator_name();
    const Operator* op = context_.GetOperator(name);
    CHECK(op != nullptr) << "Operator " << name << " was not registered.";

    // Register operation result values with 'SsaNames'.
    auto operation = std::make_unique<Operation>(*op);
    value::OperationResult operation_result(*operation, "out");
    auto operation_result_value = Value(operation_result);
    ssa_names_.GetOrCreateID(operation_result_value);
    operations_.insert({operation_it.id(), std::move(operation)});
    PreVisit(operation_proto);
  }
  for (const auto& result_it : block_proto.results().values()) {
    PreVisit(result_it.second);
  }
}

void ProtoToIR::PreVisit(const proto::Operation& operation_proto) {
  for (const auto& input_proto : operation_proto.inputs()) {
    PreVisit(input_proto);
  }
  for (const auto& attribute_it : operation_proto.attributes().attributes()) {
    PreVisit(attribute_it.second);
  }
  // TODO(#619): PreVisit(impl_module);
}

void ProtoToIR::PreVisit(const proto::Value& value_proto) {
  if (value_proto.has_any_value()) {
    PreVisit(value_proto.any_value());
  } else if (value_proto.has_operation_result_value()) {
    PreVisit(value_proto.operation_result_value());
  } else if (value_proto.has_block_argument_value()) {
    PreVisit(value_proto.block_argument_value());
  } else {
    CHECK(false) << "Unexpected variant of Value from IR";
  }
}

void ProtoToIR::PreVisit(const proto::AnyValue&) {
  // Any is empty.
}

void ProtoToIR::PreVisit(
    const proto::BlockArgumentValue& block_argument_proto) {
  // Nothing to do here.
}

void ProtoToIR::PreVisit(
    const proto::OperationResultValue& operation_argument_proto) {
  // Nothing to do here.
}

void ProtoToIR::PreVisit(const proto::AttributePayload& attribute_payload) {
  // Nothing to do here.
}

void ProtoToIR::PostVisit(Module& module, const proto::Module& module_proto) {
  for (const auto& block_it : module_proto.blocks()) {
    auto find_block_ptr = blocks_.find(block_it.id());
    CHECK(find_block_ptr != blocks_.end())
        << "This block builder should be pre-constructed.";
    auto& builder = find_block_ptr->second;
    PostVisit(builder, block_it.block());
    module.AddBlock(builder.build());
  }
  // Register the module with 'SsaNames'.
  ssa_names_.GetOrCreateID(module);
}

void ProtoToIR::PostVisit(BlockBuilder& block,
                          const proto::Block& block_proto) {
  for (const auto& operation_it : block_proto.operations()) {
    auto find_operation_ptr = operations_.find(operation_it.id());
    CHECK(find_operation_ptr != operations_.end())
        << "This operation should be pre-constructed.";
    auto op = std::move(find_operation_ptr->second);
    PostVisit(*op, operation_it.operation());
    block.AddOperation(std::move(op));
  }
  for (const auto& result_it : block_proto.results().values()) {
    PostVisit(result_it.second);
  }
  // Register the module with 'SsaNames'.
  ssa_names_.GetOrCreateID(*block.GetBlockPtr());
}

void ProtoToIR::PostVisit(Operation& op,
                          const proto::Operation& operation_proto) {
  for (const auto& input_proto : operation_proto.inputs()) {
    PostVisit(input_proto);
  }
  for (const auto& attribute_it : operation_proto.attributes().attributes()) {
    PostVisit(attribute_it.second);
  }
  // TODO(#619): impl_module
}

void ProtoToIR::PostVisit(const proto::Value& value_proto) {
  if (value_proto.has_any_value()) {
    PostVisit(value_proto.any_value());
  } else if (value_proto.has_operation_result_value()) {
    PostVisit(value_proto.operation_result_value());
  } else if (value_proto.has_block_argument_value()) {
    PostVisit(value_proto.block_argument_value());
  } else {
    CHECK(false) << "Unexpected variant of Value from IR";
  }
}

void ProtoToIR::PostVisit(const proto::AnyValue&) {
  // Any is empty.
}

void ProtoToIR::PostVisit(
    const proto::BlockArgumentValue& block_argument_proto) {
  // Nothing to do here.
}

void ProtoToIR::PostVisit(
    const proto::OperationResultValue& operation_argument_proto) {
  // Nothing to do here.
}

void ProtoToIR::PostVisit(const proto::AttributePayload& attribute_payload) {
  // Nothing to do here.
}

void ProtoToIR::ToIR(const proto::Module& module_proto) {
  for (const auto& block_it : module_proto.blocks()) {
    ToIR(block_it.id(), block_it.block());
  }
}

void ProtoToIR::ToIR(ProtoToIR::ID block_id, const proto::Block& block_proto) {
  auto find_block_ptr = blocks_.find(block_id);
  CHECK(find_block_ptr != blocks_.end())
      << "This block should be pre-constructed.";
  auto& block_builder = find_block_ptr->second;
  // TODO(#619): Consider preserving inputs/outputs.
  for (const auto& result_it : block_proto.results().values()) {
    block_builder.AddResult(result_it.first, ToIR(result_it.second));
  }
  // Also construct the block's children in place.
  for (const auto& operation_it : block_proto.operations()) {
    const auto& operation_id = operation_it.id();
    const auto& operation_proto = operation_it.operation();
    auto operation_ir_it = operations_.find(operation_id);
    CHECK(operation_ir_it != operations_.end())
        << "Operation " << operation_id << "("
        << operation_proto.operator_name() << ") was not registered.";

    Operation* operation = operation_ir_it->second.get();
    CHECK(operation != nullptr) << "Operation ptr is empty!";
    ToIR(*operation, operation_proto);
  }
}

void ProtoToIR::ToIR(Operation& operation,
                     const proto::Operation& operation_proto) {
  // TODO(#619): Consider preserving results and impl_module.
  for (const auto& input_proto : operation_proto.inputs()) {
    operation.AddInput(ToIR(input_proto));
  }
  for (const auto& attribute_it : operation_proto.attributes().attributes()) {
    operation.AddAttribute(attribute_it.first, ToIR(attribute_it.second));
  }
}

Value ProtoToIR::ToIR(const proto::Value& value_proto) {
  std::optional<Value> value;
  if (value_proto.has_any_value()) {
    value = Value(ToIR(value_proto.any_value()));
  } else if (value_proto.has_operation_result_value()) {
    value = Value(ToIR(value_proto.operation_result_value()));
  } else if (value_proto.has_block_argument_value()) {
    value = Value(ToIR(value_proto.block_argument_value()));
  }
  CHECK(value) << "Value not constructed due to unexpected variant of Value from IR.";
  // Register the value with 'SsaNames'.
  ssa_names_.GetOrCreateID(*value);
  return *value;
}

value::Any ProtoToIR::ToIR(const proto::AnyValue&) {
  value::Any any_proto;
  // Any is empty.
  return any_proto;
}

value::BlockArgument ProtoToIR::ToIR(
    const proto::BlockArgumentValue& block_argument_proto) {
  const auto id = block_argument_proto.block_id();
  auto block_it = blocks_.find(id);
  CHECK(block_it != blocks_.end()) << "Could not find block with id " << id;
  Block* block = block_it->second.GetBlockPtr();
  return value::BlockArgument(*block,
                              block_argument_proto.block_argument_name());
}

value::OperationResult ProtoToIR::ToIR(
    const proto::OperationResultValue& operation_argument_proto) {
  const auto id = operation_argument_proto.operation_id();
  auto operation_it = operations_.find(id);
  CHECK(operation_it != operations_.end()) << "Could not find operation with id " << id;
  Operation* operation = operation_it->second.get();
  return value::OperationResult(*operation,
                                operation_argument_proto.output_name());
}

Attribute ProtoToIR::ToIR(const proto::AttributePayload& attribute_payload) {
  if (attribute_payload.has_double_payload()) {
    return Attribute::Create<DoubleAttribute>(attribute_payload.double_payload());
  }
  if (attribute_payload.has_int64_payload()) {
    return Attribute::Create<Int64Attribute>(attribute_payload.int64_payload());
  }
  if (!attribute_payload.has_string_payload()) {
    CHECK(false) << "Unexpected variant of Attribute Payload from IR";
  }
  return Attribute::Create<StringAttribute>(attribute_payload.string_payload());
}
}  // namespace raksha::ir
