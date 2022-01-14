#ifndef SRC_IR_TYPES_ENTITY_TYPE_H_
#define SRC_IR_TYPES_ENTITY_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/types/schema.h"
#include "src/ir/types/type.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"

namespace raksha::ir::types {

class EntityType : public TypeBase {
 public:
  explicit EntityType(Schema schema) : schema_(std::move(schema)) {}

  TypeBase::Kind kind() const override { return TypeBase::Kind::kEntity; }

  raksha::ir::AccessPathSelectorsSet
  GetAccessPathSelectorsSet() const override {
    return schema_.GetAccessPathSelectorsSet();
  }

  const Schema& schema() const { return schema_; }

 private:
  Schema schema_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_ENTITY_TYPE_H_
