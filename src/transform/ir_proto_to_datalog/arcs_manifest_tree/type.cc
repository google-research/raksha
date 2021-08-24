#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"

#include <memory>

#include "src/common/logging/logging.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/entity_type.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/primitive_type.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

// Delegate to the various CreateFromProto implementations on the base types
// depending upon which specific type is contained within the TypeProto.
std::unique_ptr<Type> Type::CreateFromProto(const arcs::TypeProto &type_proto) {
  CHECK(!type_proto.optional())
    << "Optional types are currently unimplemented.";
  CHECK(!type_proto.has_refinement())
    << "Type refinements are currently unimplemented.";
  switch (type_proto.data_case()) {
    case arcs::TypeProto::DATA_NOT_SET:
      LOG(FATAL) << "Found a TypeProto with an unset specific type.";
    case arcs::TypeProto::kPrimitive:
      return std::make_unique<PrimitiveType>(
          PrimitiveType::CreateFromProto(type_proto.primitive()));
    case arcs::TypeProto::kEntity:
      return std::make_unique<EntityType>(
          EntityType::CreateFromProto(type_proto.entity()));
    default:
      LOG(FATAL)
          << "Found unimplemented type. Only Primitive and Entity "
             "types are currently implemented.";
    }
    CHECK(false) << "Unreachable!";
  }

}  // namespace raksha::transform::arcs_manifest_tree
