#include "third_party/arcs/proto/manifest.pb.h"

#include <cstdint>
#include <sstream>
#include <unordered_map>

// TODO: Do something dynamic to prevent collisons with user-provided names.
const char * const generated_recipe_prefix = "$GENERATED_RECIPE_";
// TODO: Get rid of this mutable global.
uint64_t next_generated_recipe_name_num = 0;

static std::string get_recipe_name(const arcs::RecipeProto &recipe) {
  const std::string &original_recipe_name = recipe.name();

  // If the original recipe name is not empty, use the user-provided recipe
  // name.
  if (!original_recipe_name.empty()) {
    return original_recipe_name;
  }

  std::ostringstream recipe_name_builder;
  recipe_name_builder
    << generated_recipe_prefix
    << next_generated_recipe_name_num;
  ++next_generated_recipe_name_num;

  return recipe_name_builder.str();
}

void read_from_manifest(const arcs::ManifestProto &manifestProto) {

  // TODO: Import abseil and use it here.
  std::unordered_map<std::string, const arcs::ParticleSpecProto>
    particle_spec_name_to_proto;

  for (
    const arcs::ParticleSpecProto &particle_spec_proto :
    manifestProto.particle_specs())
  {
    auto result_pair =
      particle_spec_name_to_proto.insert(
        {particle_spec_proto.name(), particle_spec_proto});
    bool const name_unique = !result_pair.second;
    // TODO: Use a friendlier method to enforce particle spec name uniqueness.
    assert(name_unique);
  }

  for (const arcs::RecipeProto &recipe : manifestProto.recipes()) {
    std::string const recipe_name = get_recipe_name(recipe);
    std::unordered_map<std::string, const arcs::HandleProto>
      handle_name_to_proto;

    for (const arcs::HandleProto &handle : recipe.handles()) {
      const std::string &handle_name = handle.name();
      // TODO: Do something better here.
      assert(!handle_name.empty());

      auto result_pair = handle_name_to_proto.insert({handle_name, handle});

      bool const name_collision = !result_pair.second;
      // TODO: Do something better here.
      assert(!name_collision);
    }

    for (const arcs::ParticleProto &particle : recipe.particles()) {
      std::string const &spec_name = particle.spec_name();
      // TODO: Do something better here.
      assert(!spec_name.empty());

      auto find_result = particle_spec_name_to_proto.find(spec_name);

      // TODO: Do something better here.
      assert(find_result != particle_spec_name_to_proto.end());

      const arcs::ParticleSpecProto &particle_spec_proto = find_result->second;

      
    }

  }
}
