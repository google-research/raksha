//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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

#include "src/ir/proto/particle_spec.h"

#include <memory>

#include "src/ir/access_path_root.h"
#include "src/ir/attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/handle_connection_spec.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/particle_spec.h"
#include "src/ir/proto/derives_from_claim.h"
#include "src/ir/proto/handle_connection_spec.h"
#include "src/ir/proto/operators.h"
#include "src/ir/proto/predicate.h"
#include "src/ir/proto/tag_check.h"
#include "src/ir/proto/tag_claim.h"
#include "src/ir/value.h"
#include "src/utils/map_iter.h"

namespace raksha::ir::proto {

std::unique_ptr<ParticleSpec> Decode(
    types::TypeFactory &type_factory,
    const arcs::ParticleSpecProto &particle_spec_proto) {
  std::string name = particle_spec_proto.name();
  CHECK(!name.empty()) << "Expected particle spec to have a name.";

  std::vector<HandleConnectionSpec> handle_connection_specs;
  for (const arcs::HandleConnectionSpecProto &hcs_proto :
       particle_spec_proto.connections()) {
    handle_connection_specs.push_back(proto::Decode(type_factory, hcs_proto));
  }

  std::vector<TagClaim> tag_claims;
  std::vector<DerivesFromClaim> derives_from_claims;
  for (const arcs::ClaimProto &claim : particle_spec_proto.claims()) {
    switch (claim.claim_case()) {
      case arcs::ClaimProto::kDerivesFrom: {
        // TODO(#120): Also attach the particle name to these DerivesFrom
        //  claims to allow authorization logic to mediate whether edges are
        //  drawn for these claims or not.
        derives_from_claims.push_back(proto::Decode(claim.derives_from()));
        continue;
      }
      case arcs::ClaimProto::kAssume: {
        std::vector<TagClaim> current_assume_claims =
            proto::Decode(name, claim.assume());
        tag_claims.insert(
            tag_claims.end(),
            std::make_move_iterator(current_assume_claims.begin()),
            std::make_move_iterator(current_assume_claims.end()));
        continue;
      }
      default: {
        LOG(FATAL) << "Unexpected claim variant.";
      }
    }
  }

  std::vector<TagCheck> checks;
  for (const arcs::CheckProto &check : particle_spec_proto.checks()) {
    checks.push_back(proto::Decode(check));
  }

  return ParticleSpec::Create(
      std::move(name), std::move(checks), std::move(tag_claims),
      std::move(derives_from_claims), std::move(handle_connection_specs));
}

const Operation &Decode(IRContext &ir_context, BlockBuilder &parent_builder,
                        const arcs::ParticleSpecProto &particle_spec_proto) {
  std::unique_ptr<ParticleSpec> particle_spec =
      Decode(ir_context.type_factory(), particle_spec_proto);
  const Operator &particle_spec_operator =
      ir_context.GetOperator(arcs::operators::kParticleSpec);
  std::string name = particle_spec_proto.name();
  CHECK(!name.empty()) << "Expected particle spec to have a name.";

  BlockBuilder builder;
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
  for (const auto &[name, spec] : particle_spec->handle_connection_specs()) {
    // Add inputs and outputs
    if (spec.reads()) {
      inputs.push_back(name);
      builder.AddInput(name, spec.type());
    }
    if (spec.writes()) {
      outputs.push_back(name);
      builder.AddOutput(name, spec.type());
    }
  }
  builder.AddImplementation([&ir_context, &inputs, &particle_spec, &outputs](
                                auto &builder, auto &block) {
    NamedValueMap input_values;
    NamedValueMap results;
    // First each output is Merge(ANY, inputs).
    input_values.insert({std::string("arg0"), Value(value::Any())});
    for (size_t i = 0; i < inputs.size(); ++i) {
      std::string input_arg_name = absl::StrFormat("arg%d", i + 1);
      input_values.insert(
          {input_arg_name, Value(value::BlockArgument(block, inputs[i]))});
    }
    for (const auto &output : outputs) {
      const Operation &output_operation = builder.AddOperation(
          ir_context.GetOperator(arcs::operators::kMerge), {}, input_values);
      results.insert(
          {output, Value(value::OperationResult(output_operation, "result"))});
    }
    // Handle derives from claims.
    for (const auto &derives_from_claim :
         particle_spec->derives_from_claims()) {
      const AccessPath &source = derives_from_claim.source();
      const auto *source_root = CHECK_NOTNULL(
          source.root().GetIf<HandleConnectionSpecAccessPathRoot>());
      const Operation &read_source = builder.AddOperation(
          ir_context.GetOperator(arcs::operators::kReadAccessPath),
          {{"path", StringAttribute::Create(source.selectors().ToString())}},
          {{"input", Value(value::BlockArgument(
                         block, source_root->handle_connection_spec_name()))}});
      const AccessPath &target = derives_from_claim.target();
      const auto *target_root = CHECK_NOTNULL(
          target.root().GetIf<HandleConnectionSpecAccessPathRoot>());
      std::string target_path =
          absl::StrCat(target_root->handle_connection_spec_name(),
                       target.selectors().ToString());
      Value previous_value =
          results.at(target_root->handle_connection_spec_name());
      const Operation &write_target = builder.AddOperation(
          ir_context.GetOperator(arcs::operators::kUpdateAccessPath),
          {{"path", StringAttribute::Create(target.selectors().ToString())}},
          {{"input", previous_value},
           {"value", Value(value::OperationResult(read_source, "result"))}});
      Value result = Value(value::OperationResult(write_target, "result"));
      results.emplace(target_root->handle_connection_spec_name(), result)
          .first->second = result;
    }
    // Handle tag claims.
    for (const auto &claim : particle_spec->tag_claims()) {
      const AccessPath &target = claim.access_path();
      const auto *target_root = CHECK_NOTNULL(
          target.root().GetIf<HandleConnectionSpecAccessPathRoot>());
      Value previous_value =
          results.at(target_root->handle_connection_spec_name());
      const Operation &claim_operation = builder.AddOperation(
          ir_context.GetOperator(arcs::operators::kClaim),
          {{"path", StringAttribute::Create(target.selectors().ToString())},
           {"tag", StringAttribute::Create(claim.tag())},
           {"is_present",
            Int64Attribute::Create(claim.claim_tag_is_present())}},
          {{"input", previous_value}});
      Value result = Value(value::OperationResult(claim_operation, "result"));
      results.emplace(target_root->handle_connection_spec_name(), result)
          .first->second = result;
    }
    for (const auto &[name, value] : results) {
      builder.AddResult(name, value);
    }
  });
  // Tag Checks
  auto module = std::make_unique<Module>();
  module->AddBlock(builder.build());

  return parent_builder.AddOperation(
      particle_spec_operator,
      NamedAttributeMap({{"name", StringAttribute::Create(name)}}),
      NamedValueMap({}), std::move(module));
}

}  // namespace raksha::ir::proto
