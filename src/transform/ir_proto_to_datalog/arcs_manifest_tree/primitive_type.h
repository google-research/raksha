#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_

#include "src/ir/selector_access_path_set.h"

#include "absl/container/flat_hash_set.h"

namespace raksha::transform::arcs_manifest_tree {

// TODO: Add the various kinds of primitive type.
// Right now, the most important thing about primitive types is that they
// terminate an access path. For now, we've left them empty; we should fill
// them in in the future.
class PrimitiveType {
 public:
  // For now, a primitive type has no members and a trivial constructor. This
  // will change as we add more cases to this translator in the future.
  explicit PrimitiveType() {}

  // A primitive type provides no access paths, so return an empty set.
  raksha::ir::SelectorAccessPathSet GetAccessPaths() const {
    return raksha::ir::SelectorAccessPathSet();
  }
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_
