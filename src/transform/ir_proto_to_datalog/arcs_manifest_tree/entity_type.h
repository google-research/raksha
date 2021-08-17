#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_

#include "schema.h"
#include "src/ir/selector_access_path.h"
#include "src/ir/selector_access_path_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

class EntityType {
 public:
  static absl::StatusOr<EntityType> Create(
      const arcs::EntityTypeProto &entity_proto);

  raksha::ir::SelectorAccessPathSet GetAccessPaths() const;

 private:
  EntityType(const Schema schema) : schema_(schema) {}

  Schema schema_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_
