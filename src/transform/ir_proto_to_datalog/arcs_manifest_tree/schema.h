#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_

#include "type.h"
#include "src/ir/selector_access_path_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/types/optional.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

class Schema {
 public:
  static absl::StatusOr<Schema> Create(const arcs::SchemaProto &schema_proto);

  raksha::ir::SelectorAccessPathSet GetAccessPaths() const;
 private:
  explicit Schema(
      const absl::optional<std::string> name,
      const absl::flat_hash_map<std::string, Type> fields)
      : name_(name), fields_(fields) {}

  absl::optional<std::string> name_;
  absl::flat_hash_map<std::string, Type> fields_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_
