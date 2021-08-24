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
  explicit EntityType(Schema schema) : schema_(std::move(schema)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const {
    return schema_.GetAccessPathSelectorsSet();
  }

 private:
  Schema schema_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_ENTITY_TYPE_H_
