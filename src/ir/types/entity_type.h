#ifndef SRC_IR_TYPES_ENTITY_TYPE_H_
#define SRC_IR_TYPES_ENTITY_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "absl/hash/hash.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/schema.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class EntityType : public TypeBase {
 public:
  explicit EntityType(const Schema& schema) : schema_(&schema) {}

  TypeBase::Kind kind() const override { return TypeBase::Kind::kEntity; }

  raksha::ir::AccessPathSelectorsSet
  GetAccessPathSelectorsSet() const override {
    return schema_->GetAccessPathSelectorsSet();
  }

  const Schema& schema() const { return *schema_; }

 private:
  const Schema* schema_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_ENTITY_TYPE_H_
