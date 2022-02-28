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
//----------------------------------------------------------------------------

#ifndef SRC_IR_PARTICLE_SPEC_H_
#define SRC_IR_PARTICLE_SPEC_H_

#include <string>

#include "absl/container/flat_hash_map.h"
#include "src/common/logging/logging.h"
#include "src/ir/derives_from_claim.h"
#include "src/ir/edge.h"
#include "src/ir/handle_connection_spec.h"
#include "src/ir/predicate.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"

namespace raksha::ir {

// A class representing a ParticleSpec. This contains the relevant
// information for the dataflow graph: checks, claims, and edges within the
// ParticleSpec. All such information uses this ParticleSpec as the root.
// This means that it cannot be printed to Datalog facts until these items
// are instantiated with a fully-instantiated root.
class ParticleSpec {
 public:
  static std::unique_ptr<ParticleSpec> Create(
      std::string name, std::vector<TagCheck> checks,
      std::vector<TagClaim> tag_claims,
      std::vector<DerivesFromClaim> derives_from_claims,
      std::vector<HandleConnectionSpec> handle_connection_specs);

  const std::string &name() const { return name_; }
  const std::vector<TagCheck> &checks() const { return checks_; }
  const std::vector<TagClaim> &tag_claims() const { return tag_claims_; }
  const std::vector<DerivesFromClaim> &derives_from_claims() const {
    return derives_from_claims_;
  }
  const absl::flat_hash_map<std::string, HandleConnectionSpec>
      &handle_connection_specs() const {
    return handle_connection_specs_;
  }
  const std::vector<Edge> &edges() const { return edges_; }

  const HandleConnectionSpec &getHandleConnectionSpec(
      const absl::string_view hcs_name) const {
    auto find_res = handle_connection_specs_.find(hcs_name);
    CHECK(find_res != handle_connection_specs_.end())
        << "Could not find a HandleConnectionSpec with name " << hcs_name
        << " in ParticleSpec " << name_ << ".";
    return find_res->second;
  }

 private:
  ParticleSpec(std::string name, std::vector<TagCheck> checks,
               std::vector<TagClaim> tag_claims,
               std::vector<DerivesFromClaim> derives_from_claims,
               std::vector<HandleConnectionSpec> handle_connection_specs)
      : name_(std::move(name)),
        checks_(std::move(checks)),
        tag_claims_(std::move(tag_claims)),
        derives_from_claims_(std::move(derives_from_claims)) {
    for (HandleConnectionSpec &handle_connection_spec :
         handle_connection_specs) {
      std::string hcs_name = handle_connection_spec.name();
      auto ins_res = handle_connection_specs_.insert(
          {std::move(hcs_name), std::move(handle_connection_spec)});
      CHECK(ins_res.second)
          << "Found two HandleConnectionSpecs with same name.";
    }
    GenerateEdges();
  }

  // Generate the edges between HandleConnectionSpecs within this ParticleSpec.
  void GenerateEdges();

  // The name of this ParticleSpec.
  std::string name_;
  // The checks this ParticleSpec performs upon its AccessPaths. All such
  // checks are made upon uninstantiated AccessPaths.
  std::vector<TagCheck> checks_;
  // The claims this ParticleSpec performs upon its AccessPaths. All such
  // claims are made upon uninstantiated AccessPaths.
  std::vector<TagClaim> tag_claims_;
  // Claims that a particular AccessPath in this ParticleSpec derives from
  // some other AccessPath.
  std::vector<DerivesFromClaim> derives_from_claims_;
  // The dataflow edges within this ParticleSpec connecting its
  // HandleConnectionSpecs. These edges are all between uninstantiated
  // AccessPaths.
  std::vector<Edge> edges_;
  // A map of HandleConnectionSpec names to HandleConnectionSpecs.
  absl::flat_hash_map<std::string, HandleConnectionSpec>
      handle_connection_specs_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_PARTICLE_SPEC_H_
