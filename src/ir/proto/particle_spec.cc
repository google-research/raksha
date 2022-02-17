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

#include "src/ir/handle_connection_spec.h"
#include "src/ir/ir_context.h"
#include "src/ir/particle_spec.h"
#include "src/ir/proto/derives_from_claim.h"
#include "src/ir/proto/handle_connection_spec.h"
#include "src/ir/proto/predicate.h"
#include "src/ir/proto/tag_check.h"
#include "src/ir/proto/tag_claim.h"

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

std::unique_ptr<Operation> Decode(
    IRContext &ir_context, const arcs::ParticleSpecProto &particle_spec_proto) {
  std::string name = particle_spec_proto.name();
  CHECK(!name.empty()) << "Expected particle spec to have a name.";

  std::vector<HandleConnectionSpec> handle_connection_specs;
  for (const arcs::HandleConnectionSpecProto &hcs_proto :
       particle_spec_proto.connections()) {
    handle_connection_specs.push_back(
        proto::Decode(ir_context.type_factory(), hcs_proto));
  }
  BlockBuilder builder;
  for (const auto &handle_spec : handle_connection_specs) {
  }

  return nullptr;
}

}  // namespace raksha::ir::proto
