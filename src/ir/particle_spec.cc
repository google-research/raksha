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

#include "src/ir/particle_spec.h"

namespace raksha::ir {

std::unique_ptr<ParticleSpec> ParticleSpec::Create(
    std::string name, std::vector<TagCheck> checks,
    std::vector<TagClaim> tag_claims,
    std::vector<DerivesFromClaim> derives_from_claims,
    std::vector<HandleConnectionSpec> handle_connection_specs,
    std::unique_ptr<PredicateArena> predicate_arena) {
  return std::unique_ptr<ParticleSpec>(new ParticleSpec(
      std::move(name), std::move(checks), std::move(tag_claims),
      std::move(derives_from_claims), std::move(handle_connection_specs),
      std::move(predicate_arena)));
}

void ParticleSpec::GenerateEdges() {
  // First, iterate over the DerivesFrom claims to see which AccessPaths
  // explicitly derive from some group of inputs. Draw the edges implied by
  // those claims.
  absl::flat_hash_set<AccessPath> derives_from_targets;
  for (const DerivesFromClaim &derives_from_claim : derives_from_claims_) {
    derives_from_targets.insert(derives_from_claim.target());
    edges_.push_back(derives_from_claim.GetAsEdge());
  }

  // Now that we have handle the explicit edges, draw the default-dataflow
  // edges (ie, assume that all inputs flow to all outputs without an
  // explicit DerivesFrom claim).
  // Find all of the access paths that are input to the particle spec
  // and output from the particle spec which did not have an explicit
  // derivation.
  std::vector<AccessPath> input_access_paths;
  std::vector<AccessPath> default_derivation_output_access_paths;
  for (const auto &name_hcs_pair : handle_connection_specs_) {
    const HandleConnectionSpec &connection_spec = name_hcs_pair.second;
    std::vector<AccessPath> access_paths =
        connection_spec.GetAccessPaths(name_);
    // While we want to consider all read HandleConnectionSpecs as inputs, we
    // want to consider only those which are written and do not have explicit
    // DerivesFrom claims as outputs for drawing default dataflow edges.
    if (connection_spec.writes()) {
      for (AccessPath &access_path : access_paths) {
        if (derives_from_targets.contains(access_path)) continue;
        // Note: we cannot move here because this connection spec may also be
        // read. We could check whether it is not read and move in that case,
        // but that's too ugly to bother with unless we get data that this is
        // performance-critical.
        default_derivation_output_access_paths.push_back(access_path);
      }
    }

    if (connection_spec.reads()) {
      input_access_paths.insert(
          input_access_paths.end(),
          std::make_move_iterator(access_paths.begin()),
          std::make_move_iterator(access_paths.end()));
    }
  }

  // Now that we have populated all of the access paths, draw the edges for
  // default dataflow.
  // TODO(#99): Right now, we draw edges from all input access paths to all
  // output access paths. This is quadratic; we should draw all edges without
  // explicit DerivesFrom claims to and from a nexus AccessPath to produce a
  // linear number of edges instead.
  // Note: we iterate over output paths in the outer loop and input edges in
  // the inner loop. We do this because it is entirely possible that there
  // was a DerivesFrom claim for each output, making the
  // default_derivation_output_access_paths empty. Putting the outputs in the
  // outer loop allows us to do 0 iterations in that case instead of I, where
  // I is the number of input_access_paths.
  for (const AccessPath &output : default_derivation_output_access_paths) {
    for (const AccessPath &input : input_access_paths) {
      edges_.push_back(Edge(input, output));
    }
  }
}

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree
