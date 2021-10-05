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

#ifndef SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_
#define SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_

#include <string>

#include "absl/container/flat_hash_map.h"
#include "src/common/logging/logging.h"
#include "src/ir/derives_from_claim.h"
#include "src/ir/edge.h"
#include "src/ir/predicate.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"
#include "src/ir/proto/predicate.h"
#include "src/xform_to_datalog/arcs_manifest_tree/handle_connection_spec.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

// This struct is used to return the members of a ParticleSpec after being
// instantiated.
struct InstantiatedParticleSpecFacts {
  std::vector<raksha::ir::TagClaim> tag_claims;
  std::vector<raksha::ir::TagCheck> checks;
  std::vector<raksha::ir::Edge> edges;
};

// A class representing a ParticleSpec. This contains the relevant
// information for the dataflow graph: checks, claims, and edges within the
// ParticleSpec. All such information uses this ParticleSpec as the root.
// This means that it cannot be printed to Datalog facts until these items
// are instantiated with a fully-instantiated root.
class ParticleSpec {
 public:

  const std::string &name() const { return name_; }

  const std::vector<raksha::ir::TagCheck> &checks() const { return checks_; }

  const std::vector<raksha::ir::TagClaim> &tag_claims() const {
    return tag_claims_;
  }

  const std::vector<raksha::ir::Edge> &edges() const { return edges_; }

  const HandleConnectionSpec &getHandleConnectionSpec(
      const absl::string_view hcs_name) const {
    auto find_res = handle_connection_specs_.find(hcs_name);
    CHECK(find_res != handle_connection_specs_.end())
      << "Could not find a HandleConnectionSpec with name " << hcs_name
      << " in ParticleSpec " << name_ << ".";
    return find_res->second;
  }

  InstantiatedParticleSpecFacts BulkInstantiate(
      const absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>
          &instantiation_map) const {
    InstantiatedParticleSpecFacts result;
    for (const raksha::ir::TagCheck &check : checks_) {
      result.checks.push_back(check.BulkInstantiate(instantiation_map));
    }
    for (const raksha::ir::TagClaim &claim : tag_claims_) {
      result.tag_claims.push_back(claim.BulkInstantiate(instantiation_map));
    }
    for (const raksha::ir::Edge &edge : edges_) {
      result.edges.push_back(edge.BulkInstantiate(instantiation_map));
    }
    return result;
  }

  friend class ParticleSpecRegistry;

 private:
  ParticleSpec(
      std::string name,
      std::vector<raksha::ir::TagCheck> checks,
      std::vector<raksha::ir::TagClaim> tag_claims,
      std::vector<raksha::ir::DerivesFromClaim> derives_from_claims,
      std::vector<HandleConnectionSpec> handle_connection_specs,
      raksha::ir::proto::PredicateDecoder predicate_decoder)
      : name_(std::move(name)), checks_(std::move(checks)),
        tag_claims_(std::move(tag_claims)),
        derives_from_claims_(std::move(derives_from_claims)),
        predicate_decoder_(std::move(predicate_decoder)){
    for (HandleConnectionSpec &handle_connection_spec :
      handle_connection_specs) {
      std::string hcs_name = handle_connection_spec.name();
      auto ins_res = handle_connection_specs_.insert({
        std::move(hcs_name), std::move(handle_connection_spec) });
      CHECK(ins_res.second)
        << "Found two HandleConnectionSpecs with same name.";
    }
    GenerateEdges();
  }

  // Create the ParticleSpec from a ParticleSpecProto. Private to make all
  // constructions go through the ParticleSpecRegistry.
  static ParticleSpec CreateFromProto(
      const arcs::ParticleSpecProto &particle_spec_proto);

  // Generate the edges between HandleConnectionSpecs within this ParticleSpec.
  void GenerateEdges();

  // The name of this ParticleSpec.
  std::string name_;
  // The checks this ParticleSpec performs upon its AccessPaths. All such
  // checks are made upon uninstantiated AccessPaths.
  std::vector<raksha::ir::TagCheck> checks_;
  // The claims this ParticleSpec performs upon its AccessPaths. All such
  // claims are made upon uninstantiated AccessPaths.
  std::vector<raksha::ir::TagClaim> tag_claims_;
  // Claims that a particular AccessPath in this ParticleSpec derives from
  // some other AccessPath.
  std::vector<raksha::ir::DerivesFromClaim> derives_from_claims_;
  // The dataflow edges within this ParticleSpec connecting its
  // HandleConnectionSpecs. These edges are all between uninstantiated
  // AccessPaths.
  std::vector<raksha::ir::Edge> edges_;
  // A map of HandleConnectionSpec names to HandleConnectionSpecs.
  absl::flat_hash_map<std::string, HandleConnectionSpec>
    handle_connection_specs_;
  // The PredicateDecoder object which was used to construct the predicates
  // on all of the checks on this ParticleSpec. This also owns those
  // predicates, so holding this field here causes the checks to live as long
  // as the ParticleSpec.
  raksha::ir::proto::PredicateDecoder predicate_decoder_;
};

// A ParticleSpecRegistry owns all ParticleSpecs. It acts as a factory for
// particle specs and a place to look up ParticleSpecs by name.
class ParticleSpecRegistry {
 public:

  // Get a ParticleSpec with a particular name. Errors if there is no
  // particle spec with that name.
  const ParticleSpec &GetParticleSpec(absl::string_view particle_spec_name) {
    auto find_res = particle_specs_.find(particle_spec_name);
    CHECK(find_res != particle_specs_.end())
      << "No particle spec with name " << particle_spec_name;
    return *find_res->second;
  }

  // Constructs a ParticleSpec from a proto and enters it into the registry.
  // Errors if there is already a particle spec with that name. Returns a
  // const reference to the ParticleSpec.
  const ParticleSpec &CreateParticleSpecFromProto(
      const arcs::ParticleSpecProto &particle_spec_proto) {
    std::unique_ptr<ParticleSpec> new_particle_spec(
        new ParticleSpec(ParticleSpec::CreateFromProto(particle_spec_proto)));
    std::string particle_spec_name = new_particle_spec->name_;
    auto ins_res = particle_specs_.insert(
        { std::move(particle_spec_name), std::move(new_particle_spec) } );
    const ParticleSpec &canon_particle_spec = *ins_res.first->second;
    CHECK(ins_res.second)
      << "Found multiple particle specs with name "
      << canon_particle_spec.name();
    return canon_particle_spec;
  }

 private:
  absl::flat_hash_map<std::string, std::unique_ptr<ParticleSpec>>
    particle_specs_;
};

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree

#endif  // SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_
