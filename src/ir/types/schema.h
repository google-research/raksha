#ifndef SRC_IR_TYPES_SCHEMA_H_
#define SRC_IR_TYPES_SCHEMA_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/types/optional.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types {

class Schema {
 public:
  explicit Schema(
    absl::optional<std::string> name,
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields)
    : name_(std::move(name)), fields_(std::move(fields)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPaths() const;
 private:

  absl::optional<std::string> name_;
  absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_SCHEMA_H_
