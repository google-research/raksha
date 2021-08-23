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
  static Schema CreateFromProto(arcs::SchemaProto schema_proto) {
    auto schema_names = schema_proto.names();
    CHECK(schema_names.size() <= 1)
      << "Multiple names for a Schema not yet supported.";
    absl::optional<std::string> name;
    if (schema_names.size() == 1) {
      name = schema_names.at(0);
    }

    absl::flat_hash_map<std::string, std::unique_ptr<Type>> field_map;
    for (const auto &field_name_type_pair : schema_proto.fields()) {
      const std::string &field_name = field_name_type_pair.first;
      const arcs::TypeProto &type_proto = field_name_type_pair.second;

      field_map.insert(field_name, Type::CreateFromProto(type_proto));
    }

    return Schema(std::move(name), std::move(field_map));
  }

  explicit Schema(
    absl::optional<std::string> name,
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields)
    : name_(name), fields_(std::move(fields)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPaths() const;
 private:

  absl::optional<std::string> name_;
  absl::flat_hash_map<std::string, std::unique_ptr<Type>> fields_;
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_SCHEMA_H_
