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
#ifndef SRC_IR_PROTO_TO_IR_H_
#define SRC_IR_PROTO_TO_IR_H_

#include "google/protobuf/util/json_util.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/proto/raksha_ir.pb.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value.h"
#include "absl/status/statusor.h"

namespace raksha::ir {

class ProtoToIR {
 public:
  using ID = uint64_t;
  struct Result {
    std::unique_ptr<Module> module;
    std::unique_ptr<SsaNames> ssa_names;
  };

  static absl::StatusOr<Result> Convert(IRContext& context,
                        absl::string_view irTranslationUnitJsonProto) {
      proto::IrTranslationUnit irTranslationUnit;
      auto status = google::protobuf::util::JsonStringToMessage(irTranslationUnitJsonProto,
                                                                &irTranslationUnit);
      if (!status.ok()) {
          LOG(ERROR) << "Json IR proto is corrupt " << status;
          return absl::InvalidArgumentError("Json IR proto is corrupt");
      }

      return Convert(context, irTranslationUnit);
  }

  static Result Convert(IRContext& context,
                        const proto::IrTranslationUnit& root) {
    // Store the operators in the context.
    for (const auto& op : root.operators()) {
      auto name = op.name();
      // Register new operators.
      if (!context.IsRegisteredOperator(name)) {
        Operator op = Operator(name);
        context.RegisterOperator(std::make_unique<Operator>(std::move(op)));
      }
    }

    auto ssa_names = std::make_unique<SsaNames>();
    ProtoToIR state(context, *ssa_names);
    // Construct objects to point to during construction.
    auto module = state.PreVisit(root.top_level_module());
    // Perform construction.
    state.ToIR(root.top_level_module());
    // Move objects into their 'owning' objects.
    state.PostVisit(*module, root.top_level_module());

    // Check that the construction correctly handled ownership of all referenced
    // objects.
    for (const auto& block_it : state.blocks_) {
      CHECK(block_it.second.IsBuilt())
          << "Block " << block_it.first
          << " was not built and placed in it's module.";
    }
    for (const auto& operation_it : state.operations_) {
      CHECK(operation_it.second.get() == nullptr)
          << "Operation " << operation_it.first
          << " was not moved into it's module.";
    }
    return Result{.module = std::move(module),
                  .ssa_names = std::move(ssa_names)};
  }

 private:
  ProtoToIR(IRContext& context, SsaNames& ssa_names)
      : context_{context}, ssa_names_{ssa_names} {}

  // Disable copy (and move) semantics.
  ProtoToIR(const ProtoToIR&) = delete;
  ProtoToIR& operator=(const ProtoToIR&) = delete;

  virtual ~ProtoToIR() {}

  std::unique_ptr<Module> PreVisit(const proto::Module& module_proto);
  void PreVisit(const proto::Block& block_proto);
  void PreVisit(const proto::Operation& operation_proto);
  void PreVisit(const proto::Value& value);
  void PreVisit(const proto::AnyValue& any_value);
  void PreVisit(const proto::BlockArgumentValue& block_argument);
  void PreVisit(const proto::OperationResultValue&);
  void PreVisit(const proto::AttributePayload&);

  void ToIR(const proto::Module& module_proto);
  void ToIR(ID block_id, const proto::Block& block_proto);
  void ToIR(Operation& op, const proto::Operation& operation_proto);
  Value ToIR(const proto::Value& value);
  value::Any ToIR(const proto::AnyValue&);
  value::BlockArgument ToIR(const proto::BlockArgumentValue& block_argument);
  value::OperationResult ToIR(const proto::OperationResultValue&);
  Attribute ToIR(const proto::AttributePayload&);

  void PostVisit(Module& module, const proto::Module& module_proto);
  void PostVisit(BlockBuilder& block, const proto::Block& block_proto);
  void PostVisit(Operation& op, const proto::Operation& operation_proto);
  void PostVisit(const proto::Value& value);
  void PostVisit(const proto::AnyValue& any_value);
  void PostVisit(const proto::BlockArgumentValue& block_argument);
  void PostVisit(const proto::OperationResultValue&);
  void PostVisit(const proto::AttributePayload&);

  const IRContext& context_;
  SsaNames& ssa_names_;
  // TODO(#620): Consider an 'OperationBuilder'
  absl::flat_hash_map<ID, std::unique_ptr<Operation>> operations_;
  absl::flat_hash_map<ID, BlockBuilder> blocks_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_PROTO_TO_IR_H_
