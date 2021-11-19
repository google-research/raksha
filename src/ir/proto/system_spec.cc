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
#ifndef SRC_IR_PROTO_SYSTEM_SPEC_H_
#define SRC_IR_PROTO_SYSTEM_SPEC_H_

#include "src/ir/proto/system_spec.h"

#include <memory>

#include "src/ir/proto/particle_spec.h"
#include "src/ir/system_spec.h"

namespace raksha::ir::proto {

std::unique_ptr<SystemSpec> Decode(const arcs::ManifestProto &manifest_proto) {
  // Turn each ParticleSpecProto indicated in the manifest_proto into a
  // ParticleSpec object, which we can use directly.
  auto system_spec = std::make_unique<SystemSpec>();
  for (const arcs::ParticleSpecProto &particle_spec_proto :
       manifest_proto.particle_specs()) {
    std::unique_ptr<ir::PredicateArena> arena =
        std::make_unique<ir::PredicateArena>();
    system_spec->AddParticleSpec(
        ir::proto::Decode(std::move(arena), particle_spec_proto));
  }
  return system_spec;
}

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_SYSTEM_SPEC_H_
