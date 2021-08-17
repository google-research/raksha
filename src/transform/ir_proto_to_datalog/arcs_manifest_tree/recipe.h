#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_RECIPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_RECIPE_H_

#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/handle.h"

#include <string>
#include <cstdint>
#include <memory>

namespace raksha::transform::arcs_manifest_tree {

class Recipe {
  private:
    const std::string name_;
    absl::flat_hash_map<const absl::string_view, std::unique_ptr<Handle>>
      handles_;

    explicit Recipe(absl::string_view name, uint64_t num_handles);

  public:

    static std::unique_ptr<Recipe> Create(
      const arcs::RecipeProto &recipe_proto,
      absl::string_view guaranteed_unique_prefix,
      uint64_t recipe_num);

    // Recipe is neither copyable nor movable.
    Recipe(const Recipe &) = delete;

    Recipe &operator=(const Recipe &) = delete;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_RECIPE_H_
