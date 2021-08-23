#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

class Type {
 public:
  virtual ~Type() {}
  virtual raksha::ir::AccessPathSelectorsSet GetAccessPaths() const = 0;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_TYPE_H_
