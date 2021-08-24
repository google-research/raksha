#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/types/optional.h"
#include "src/common/logging/logging.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::transform::arcs_manifest_tree {

class Schema {
 public:
  // Create a Schema object from an Arcs manifest SchemaProto message.
  static Schema CreateFromProto(arcs::SchemaProto schema_proto);

  explicit Schema(
    absl::optional<std::string> name,
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields)
    : name_(name), fields_(std::move(fields)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPaths() const;
  arcs::SchemaProto MakeProto() const;
 private:

  absl::optional<std::string> name_;
  absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_
