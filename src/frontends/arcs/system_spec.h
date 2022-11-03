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
#ifndef SRC_FRONTENDS_ARCS_SYSTEM_SPEC_H_
#define SRC_FRONTENDS_ARCS_SYSTEM_SPEC_H_

#include "src/frontends/arcs/particle_spec.h"
#include "src/ir/types/type_factory.h"

namespace raksha::frontends::arcs {

// The specification of the entire system that is being modeled in the IR.
class SystemSpec {
 public:
  const ParticleSpec *AddParticleSpec(
      std::unique_ptr<ParticleSpec> particle_spec) {
    auto ins_res = particle_specs_.insert(
        {particle_spec->name(), std::move(particle_spec)});
    CHECK(ins_res.second) << "Tried to insert second particle spec with name "
                          << ins_res.first->first;
    return ins_res.first->second.get();
  }

  // Returns a ParticleSpec with a particular name. If there is no
  // particle spec with that name, returns nullptr.
  const ParticleSpec *GetParticleSpec(
      absl::string_view particle_spec_name) const {
    auto find_res = particle_specs_.find(particle_spec_name);
    return (find_res != particle_specs_.end()) ? find_res->second.get()
                                               : nullptr;
  }

 private:
  // The particles in the system spec.
  common::containers::HashMap<std::string, std::unique_ptr<ParticleSpec>>
      particle_specs_;
};

}  // namespace raksha::frontends::arcs

#endif  // SRC_FRONTENDS_ARCS_SYSTEM_SPEC_H_
