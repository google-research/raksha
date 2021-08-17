#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/recipe.h"
#include "src/transform/ir_proto_to_datalog/logging/context_logger.h"

#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/strings/string_view.h"

#include <memory>
#include <string>
#include <cstdint>

namespace raksha::transform::arcs_manifest_tree {

namespace logging = ::raksha::transform::logging;

Recipe::Recipe(const absl::string_view name, const uint64_t num_handles)
  : name_(name),
    handles_(num_handles)
{}

std::unique_ptr<Recipe> Recipe::Create(
    const arcs::RecipeProto &recipe_proto,
    absl::string_view guaranteed_unique_prefix,
    uint64_t recipe_num
 ) {
  std::unique_ptr<Recipe> empty_result;
  const std::string &proto_recipe_name = recipe_proto.name();
  const std::string recipe_name =
    proto_recipe_name.empty()
      ? absl::StrCat(guaranteed_unique_prefix, recipe_num)
      : proto_recipe_name;
  const std::string recipe_header_name =
      proto_recipe_name.empty()
        ? "<UNNAMED RECIPE>"
        : proto_recipe_name;

  std::unique_ptr<Recipe> result(
    new Recipe(recipe_name, recipe_proto.handles_size()));
  bool saw_error = false;
  // Open a context to have the logging constructor/destructor operate as
  // intended.
  {
    logging::LoggingContext recipe_context(
        absl::StrCat("Recipe ", proto_recipe_name));

      for (const arcs::HandleProto &handle_proto : recipe_proto.handles()) {
        std::unique_ptr<Handle> handle_ptr = Handle::Create(handle_proto);
        if (!handle_ptr) {
          saw_error = true;
          continue;
        }
        const absl::string_view handle_name = handle_ptr->GetName();
        auto insert_result = result->handles_.insert(
          {handle_name, std::move(handle_ptr)});
        const bool duplicate_name = !insert_result.second;
        if (duplicate_name) {
          logging::ContextLogger::error(
              absl::StrCat(
                  "Handle name collision on name: ",
                  handle_name,
                  "\n"));
          saw_error = true;
        }
      }
  }

  if (saw_error) {
    return empty_result;
  } else {
    return result;
  }
}


}  // namespace raksha::transform::arcs_manifest_tree
