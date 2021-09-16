#ifndef SRC_IR_TYPES_ENTITY_TYPE_H_
#define SRC_IR_TYPES_ENTITY_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/types/schema.h"
#include "src/ir/types/type.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types {

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

  Type::Kind kind() const override { return Type::Kind::kEntity; }

  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const {
    return schema_.GetAccessPathSelectorsSet();
  }

  // Create a TypeProto containing an EntityTypeProto with this type's
  // information.
  arcs::TypeProto MakeProto() const {
    arcs::EntityTypeProto entity_type_proto;
    *entity_type_proto.mutable_schema() = schema_.MakeProto();

    arcs::TypeProto type_proto;
    *type_proto.mutable_entity() = std::move(entity_type_proto);
    return type_proto;
  }

 private:
  Schema schema_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_ENTITY_TYPE_H_
