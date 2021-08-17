#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_

#include "primitive_type.h"
#include "entity_type.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "src/ir/selector_access_path_set.h"

#include "absl/container/flat_hash_set.h"
#include "absl/types/variant.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

class Type {
 public:
  static absl::StatusOr<Type> Create(const arcs::TypeProto &type_proto);

  raksha::ir::SelectorAccessPathSet GetAccessPaths() const;

 private:
  explicit Type(const PrimitiveType primitive_type)
    : specific_type_(primitive_type) {}

  explicit Type(const EntityType entity_type) : specific_type_(entity_type) {}

  absl::variant<PrimitiveType, EntityType> specific_type_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_
