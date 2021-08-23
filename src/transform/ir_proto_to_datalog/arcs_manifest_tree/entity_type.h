#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/common/logging/logging.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/schema.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

class EntityType : public Type {
 public:

  // Factory function creating an EntityType from an Arcs EntityType proto.
  static EntityType CreateFromProto(
      const arcs::EntityTypeProto &entity_type_proto) {
    CHECK(entity_type_proto.has_schema())
      << "Schema is required for Entity types.";
    return EntityType(Schema::CreateFromProto(entity_type_proto.schema()));
  }

  explicit EntityType(Schema schema) : schema_(std::move(schema)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPaths() const {
    return schema_.GetAccessPaths();
  }

 private:
  Schema schema_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_ENTITY_TYPE_H_
