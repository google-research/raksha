#include "src/xform_to_datalog/arcs_manifest_tree/particle_spec.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

namespace ir = raksha::ir;

ParticleSpec ParticleSpec::CreateFromProto(
    const arcs::ParticleSpecProto &particle_spec_proto) {
  std::string name = particle_spec_proto.name();
  CHECK(!name.empty()) << "Expected particle spec to have a name.";

  std::vector<HandleConnectionSpec> handle_connection_specs;
  for (const arcs::HandleConnectionSpecProto &hcs_proto :
      particle_spec_proto.connections()) {
    handle_connection_specs.push_back(
        HandleConnectionSpec::CreateFromProto(hcs_proto));
  }

  std::vector<ir::TagClaim> claims;
  for (const arcs::ClaimProto &claim : particle_spec_proto.claims()) {
    CHECK(!claim.has_derives_from())
      << "DerivesFrom claims not yet implemented";
    CHECK(claim.has_assume()) << "Expected claim to carry an Assume field.";
    claims.push_back(ir::TagClaim::CreateFromProto(claim.assume()));
  }

  std::vector<ir::TagCheck> checks;
  for (const arcs::CheckProto &check : particle_spec_proto.checks()) {
    checks.push_back(ir::TagCheck::CreateFromProto(check));
  }

  std::vector<ir::Edge> edges =
      GenerateEdges(name, std::move(handle_connection_specs));

  return ParticleSpec(
      std::move(name), std::move(checks), std::move(claims), std::move(edges));
}

std::vector<ir::Edge> ParticleSpec::GenerateEdges(
    std::string particle_spec_name,
    std::vector<HandleConnectionSpec> connection_specs) {
  // First, find all of the access paths that are input to the particle spec
  // and output from the particle spec.
  std::vector<ir::AccessPath> input_access_paths;
  std::vector<ir::AccessPath> output_access_paths;
  for (HandleConnectionSpec &connection_spec : connection_specs) {
    std::vector<ir::AccessPath> access_paths =
        connection_spec.GetAccessPaths(particle_spec_name);
    if (connection_spec.reads()) {
      // Note: we cannot move the access_paths here because handles can be both
      // read and written. Therefore, it may be used in the writes() case after
      // being used here. We could eliminate the potential copy with an
      // additional case for reading and writing, but the decreased readability
      // and code reuse is probably not worth the potential performance gains.
      input_access_paths.insert(
          input_access_paths.end(), access_paths.begin(), access_paths.end());
    }
    if (connection_spec.writes()) {
      output_access_paths.insert(
          output_access_paths.end(),
          std::make_move_iterator(access_paths.begin()),
          std::make_move_iterator(access_paths.end()));
    }
  }

  // Now that we have populated all of the access paths, draw the edges.
  // TODO(#99): Right now, we draw edges from all input access paths to all
  // output access paths. This is quadratic; we should draw all edges without
  // explicit DerivesFrom claims to and from a nexus AccessPath to produce a
  // linear number of edges instead.
  std::vector<ir::Edge> edges;
  for (const ir::AccessPath &input : input_access_paths) {
    for (const ir::AccessPath &output : output_access_paths) {
      edges.push_back(ir::Edge(input, output));
    }
  }
  return edges;
}

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree
