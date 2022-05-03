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
#include "src/frontends/arcs/xform_to_datalog/manifest_datalog_facts.h"

#include "absl/container/flat_hash_map.h"
#include "src/frontends/arcs/handle_connection_spec.h"
#include "src/frontends/arcs/particle_spec.h"
#include "src/frontends/arcs/proto/system_spec.h"
#include "src/ir/types/proto/type.h"
#include "src/ir/types/type_factory.h"

namespace raksha::frontends::arcs::xform_to_datalog {

namespace arcs = frontends::arcs;
namespace types = raksha::ir::types;

// Traverse the substructures of the manifest proto to create datalog fact
// objects.
// #TODO(#107): In the interest of prototyping speed, I wrote out the
//  entirety of the traversal of instantiation structures as one big
//  traversal. This works, but it is hard to read and hard to test. We should
//  break this up.
ManifestDatalogFacts ManifestDatalogFacts::CreateFromManifestProto(
    types::TypeFactory &type_factory, const arcs::SystemSpec &system_spec,
    const ::arcs::ManifestProto &manifest_proto) {
  // These collections will be used as inputs to the constructor that we
  // return from this function.
  std::vector<Particle> particle_instances;

  // This loop looks at each recipe in the manifest proto and instantiates
  // the ParticleSpecs indicated by the ParticleProtos in that recipe. It
  // additionally generates the edges in and out of the Particle to Handles
  // as indicated by the HandleConnections on the Particles.
  //
  // To allow recipes to be distinguished as a component of an AccessPath, we
  // provide each recipe a unique name. For recipes that have a name
  // provided by the user, we use the user-provided name. For those recipes
  // where the user did not provide a name, we generate a name by combining the
  // generated_recipe_name_prefix and a recipe_num counter that we increment
  // for each generated recipe name.
  const std::string generated_recipe_name_prefix = "GENERATED_RECIPE_NAME";
  uint64_t recipe_num = 0;
  for (const ::arcs::RecipeProto &recipe_proto : manifest_proto.recipes()) {
    absl::flat_hash_map<std::string, uint64_t> particle_numbers;
    absl::flat_hash_map<std::string, std::string> handle_ids;
    for (const ::arcs::HandleProto &handle_proto : recipe_proto.handles()) {
      auto insert_result = handle_ids.insert(
          {handle_proto.name(), handle_proto.id().empty() ? handle_proto.name()
                                                          : handle_proto.id()});
      CHECK(insert_result.second) << "Duplicate handle id is not expected.";
    }
    const std::string &recipe_name =
        recipe_proto.name().empty()
            ? absl::StrCat(generated_recipe_name_prefix, recipe_num++)
            : recipe_proto.name();

    // For each Particle, generate the relevant facts for that particle.
    // These fall into two categories: facts that lie within the ParticleSpec
    // that just need to be instantiated for this particular Particle, and
    // edges that connect this Particle to input/output Handles.
    for (const ::arcs::ParticleProto &particle_proto :
         recipe_proto.particles()) {
      const std::string &particle_spec_name = particle_proto.spec_name();
      CHECK(!particle_spec_name.empty())
          << "Particle with empty spec_name field not allowed.";
      uint64_t particle_num = particle_numbers[particle_spec_name]++;
      const std::string particle_name =
          (particle_num == 0)
              ? particle_spec_name
              : absl::StrCat(particle_spec_name, "#", particle_num);

      // Find the ParticleSpec referenced by this Particle. The information
      // contained in the spec will be needed for all facts produced within a
      // Particle.
      const arcs::ParticleSpec &particle_spec =
          *CHECK_NOTNULL(system_spec.GetParticleSpec(particle_spec_name));

      // Each ParticleSpec already contains lists of TagClaims, TagChecks,
      // and Edges that shall be generated for each Particle implementing that
      // Spec, but which are rooted at the ParticleSpec rather than the
      // implementing Recipe and Particle. Most of the work of generating the
      // facts for a particular Particle just comes down to swapping out the
      // ParticleSpec roots for the corresponding Particle roots. The
      // instantiation_map describing how that swapping should be done is
      // populated by iterating over all HandleConnectionProtos and, for each
      // HandleConnectionSpec and HandleConnection pair indicated, adding
      // that pair to the instantiation_map.
      arcs::DatalogPrintContext::AccessPathInstantiationMap instantiation_map;
      std::vector<arcs::Edge> particle_edges;
      for (const ::arcs::HandleConnectionProto &connection_proto :
           particle_proto.connections()) {
        const std::string &handle_spec_name = connection_proto.name();
        CHECK(!handle_spec_name.empty())
            << "Handle connection with empty name field not allowed.";
        std::string handle_name = connection_proto.handle();
        CHECK(!handle_name.empty())
            << "Handle with empty handle field not allowed.";

        arcs::HandleConnectionSpecAccessPathRoot spec_handle_root(
            particle_spec_name, handle_spec_name);
        arcs::HandleConnectionAccessPathRoot instantiated_handle_root(
            recipe_name, particle_name, handle_spec_name);

        // Set up the map to replace the HandleConnectionSpec root with the
        // HandleConnection root when instantiating the Particle.
        instantiation_map.insert(
            {arcs::AccessPathRoot(std::move(spec_handle_root)),
             arcs::AccessPathRoot(instantiated_handle_root)});

        auto find_result = handle_ids.find(handle_name);
        std::string handle_id =
            find_result == handle_ids.end() ? handle_name : find_result->second;

        arcs::HandleAccessPathRoot handle_root(recipe_name,
                                               std::move(handle_id));

        CHECK(connection_proto.has_type())
            << "Handle connection with absent type not allowed.";
        types::Type connection_type =
            types::proto::Decode(type_factory, connection_proto.type());
        ir::AccessPathSelectorsSet access_path_selectors_set =
            connection_type.GetAccessPathSelectorsSet();

        // Look up the HandleConnectionSpec to see if the handle connection
        // will read and/or write.
        const arcs::HandleConnectionSpec &handle_connection_spec =
            particle_spec.getHandleConnectionSpec(handle_spec_name);
        const bool handle_connection_reads = handle_connection_spec.reads();
        const bool handle_connection_writes = handle_connection_spec.writes();

        for (const ir::AccessPathSelectors &selectors :
             ir::AccessPathSelectorsSet::CreateAbslSet(
                 access_path_selectors_set)) {
          arcs::AccessPath handle_access_path(arcs::AccessPathRoot(handle_root),
                                              selectors);
          arcs::AccessPath handle_connection_access_path(
              arcs::AccessPathRoot(instantiated_handle_root), selectors);

          // If the handle connection reads, draw a dataflow edge from the
          // handle to the handle connection.
          if (handle_connection_reads) {
            particle_edges.push_back(
                arcs::Edge(handle_access_path, handle_connection_access_path));
          }

          // If the handle connection writes, draw a dataflow edge from the
          // handle connection to the handle.
          if (handle_connection_writes) {
            particle_edges.push_back(
                arcs::Edge(handle_connection_access_path, handle_access_path));
          }
        }
      }

      particle_instances.push_back(Particle(&particle_spec,
                                            std::move(instantiation_map),
                                            std::move(particle_edges)));
    }
  }

  return ManifestDatalogFacts(std::move(particle_instances));
}

}  // namespace raksha::frontends::arcs::xform_to_datalog
