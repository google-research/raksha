#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"

#include <glog/logging.h>
#include <memory>

#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/entity_type.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/primitive_type.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

std::unique_ptr<Type> Type::CreateFromProto(const arcs::TypeProto &type_proto) {
  CHECK(!type_proto.optional())
    << "Optional types are currently unimplemented.";
  CHECK(!type_proto.has_refinement())
    << "Type refinements are currently unimplemented.";
  switch (type_proto.data_case()) {
    case (arcs::TypeProto::DATA_NOT_SET):
      LOG(FATAL) << "Missing specific type information in TypeProto.";
    case (arcs::TypeProto::kPrimitive):
      return std::make_unique<PrimitiveType>(
          PrimitiveType::CreateFromProto(type_proto.primitive()));
    case (arcs::TypeProto::kEntity): {
      return std::make_unique<EntityType>(
          EntityType::CreateFromProto(type_proto.entity()));
    }
    default: LOG(FATAL)
      << "Found unimplemented type. Only Primitive and Entity "
         "types are currently implemented.";
  }
}

}  // namespace raksha::transform::arcs_manifest_tree
