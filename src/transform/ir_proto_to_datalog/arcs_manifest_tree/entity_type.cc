#include "entity_type.h"
#include "src/ir/selector_access_path_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

absl::StatusOr<EntityType> EntityType::Create(
    const arcs::EntityTypeProto &entity_type_proto) {
  if (!entity_type_proto.has_schema()) {
    return absl::InvalidArgumentError(
        "schema field of EntityType is required.");
  }
  const absl::StatusOr<Schema> schema =
      Schema::Create(entity_type_proto.schema());
  if (!schema.ok()) {
    return schema.status();
  }

  return EntityType(*schema);
}

raksha::ir::SelectorAccessPathSet EntityType::GetAccessPaths() const {
  return schema_.GetAccessPaths();
}

}  // namespace raksha::transform::arcs_manifest_tree
