#include "type.h"

#include "src/ir/selector_access_path.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_set.h"

namespace raksha::transform::arcs_manifest_tree {

absl::StatusOr<Type> Type::Create(const arcs::TypeProto &type_proto) {
  if (type_proto.optional()) {
    return absl::InvalidArgumentError(
        "Optional types are currently unimplemented.");
  }
  if (type_proto.has_refinement()) {
     return absl::InvalidArgumentError(
        "Type refinements are currently unimplemented.");
  }
  switch (type_proto.data_case()) {
    case (arcs::TypeProto::DATA_NOT_SET):
      return absl::InvalidArgumentError(
          "Missing specific type information in TypeProto.");
    case (arcs::TypeProto::kPrimitive):
      return Type(PrimitiveType());
    case (arcs::TypeProto::kEntity): {
      absl::StatusOr<EntityType> entity_type =
          EntityType::Create(type_proto.entity());
      if (!entity_type.ok()) {
        return entity_type.status();
      }
      return Type(*entity_type);
    }
    default: return absl::InvalidArgumentError(
        "Found unimplemented type. Only Primitive and Entity "
        "types are currently implemented.");
  }
}

struct GenericGetAccessPathSetFunction {
 public:
  template<class T>
  raksha::ir::SelectorAccessPathSet operator()(const T &specific_type) const {
    return specific_type.GetAccessPaths();
  }
};

raksha::ir::SelectorAccessPathSet Type::GetAccessPaths() const {
  return absl::visit(GenericGetAccessPathSetFunction(), specific_type_);
}

}  // namespace raksha::transform::arcs_manifest_tree
