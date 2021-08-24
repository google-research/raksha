#ifndef SRC_IR_TYPES_TYPE_H_
#define SRC_IR_TYPES_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types {

class Type {
 public:
  virtual ~Type() {}
  virtual raksha::ir::AccessPathSelectorsSet GetAccessPaths() const = 0;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_TYPE_H_
