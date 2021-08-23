#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

// TODO: Add the various kinds of primitive type.
// Right now, the most important thing about primitive types is that they
// terminate an access path. For now, we've left them empty; we should fill
// them in in the future.
class PrimitiveType : public Type {
 public:
  static PrimitiveType CreateFromProto(
      const arcs::PrimitiveTypeProto &primitive_type_proto) {
    return PrimitiveType();
  }

  // For now, a primitive type has no members and a trivial constructor. This
  // will change as we add more cases to this translator in the future.
  PrimitiveType() = default;

  // A primitive type marks the end of a single access path to be built.
  // Return a set containing an empty AccessPathSelectors object.
  raksha::ir::AccessPathSelectorsSet GetAccessPaths() const override {
    return raksha::ir::AccessPathSelectorsSet(
        { raksha::ir::AccessPathSelectors() });
  }
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PRIMITIVE_TYPE_H_
