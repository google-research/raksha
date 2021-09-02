#ifndef SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_
#define SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_

#include <string>

#include "absl/container/flat_hash_map.h"
#include "src/common/logging/logging.h"
#include "src/ir/edge.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"
#include "src/xform_to_datalog/arcs_manifest_tree/handle_connection_spec.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

// This struct is used to return the members of a ParticleSpec after being
// instantiated.
struct InstantiatedParticleSpecFacts {
  std::vector<raksha::ir::TagClaim> claims;
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

  // Create the ParticleSpec from a ParticleSpecProto.
  static ParticleSpec CreateFromProto(
      const arcs::ParticleSpecProto &particle_spec_proto);

  const std::string &name() const { return name_; }

  const std::vector<raksha::ir::TagCheck> &checks() const { return checks_; }

  const std::vector<raksha::ir::TagClaim> &claims() const { return claims_; }

  const std::vector<raksha::ir::Edge> &edges() const { return edges_; }

  InstantiatedParticleSpecFacts BulkInstantiate(
      const absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>
          &instantiation_map) const {
    InstantiatedParticleSpecFacts result;
    for (const raksha::ir::TagCheck &check : checks_) {
      result.checks.push_back(check.BulkInstantiate(instantiation_map));
    }
    for (const raksha::ir::TagClaim &claim : claims_) {
      result.claims.push_back(claim.BulkInstantiate(instantiation_map));
    }
    for (const raksha::ir::Edge &edge : edges_) {
      result.edges.push_back(edge.BulkInstantiate(instantiation_map));
    }
    return result;
  }

 private:
  ParticleSpec(
      std::string name,
      std::vector<raksha::ir::TagCheck> checks,
      std::vector<raksha::ir::TagClaim> claims,
      std::vector<raksha::ir::Edge> edges)
      : name_(std::move(name)), checks_(std::move(checks)),
        claims_(std::move(claims)), edges_(std::move(edges)) { }

  // Generate the edges between HandleConnectionSpecs within this ParticleSpec.
  static std::vector<raksha::ir::Edge> GenerateEdges(
      std::string particle_spec_name,
      std::vector<HandleConnectionSpec> connection_specs);

  // The name of this ParticleSpec.
  std::string name_;
  // The checks this ParticleSpec performs upon its AccessPaths. All such
  // checks are made upon uninstantiated AccessPaths.
  std::vector<raksha::ir::TagCheck> checks_;
  // The claims this ParticleSpec performs upon its AccessPaths. All such
  // claims are made upon uninstantiated AccessPaths.
  std::vector<raksha::ir::TagClaim> claims_;
  // The dataflow edges within this ParticleSpec connecting its
  // HandleConnectionSpecs. These edges are all between uninstantiated
  // AccessPaths.
  std::vector<raksha::ir::Edge> edges_;
};

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree

#endif  // SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_
