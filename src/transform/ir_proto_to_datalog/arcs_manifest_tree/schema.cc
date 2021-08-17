#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/schema.h"
#include "src/ir/selector_access_path_set.h"
#include "src/ir/selector_access_path.h"
#include "src/ir/selector.h"
#include "type.h"
#include "src/ir/field_selector.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/types/optional.h"

#include <string>

namespace raksha::transform::arcs_manifest_tree {

namespace ir = raksha::ir;

absl::StatusOr<Schema> Schema::Create(const arcs::SchemaProto &schema_proto) {
  // Don't handle multiple names for now.
  if (schema_proto.names_size() > 1) {
    return absl::InvalidArgumentError(
        "Schemas with multiple names are currently unimplemented.");
  }
  const absl::optional<std::string> optional_name =
      (schema_proto.names_size() == 1)
        ? schema_proto.names().at(0)
        : absl::optional<std::string>();

  absl::flat_hash_map<std::string, Type> field_map;
  for (const std::pair<std::string, arcs::TypeProto> field_pair :
    schema_proto.fields()) {
    const std::string field_name = field_pair.first;
    const arcs::TypeProto type_proto = field_pair.second;

    if (field_name.empty()) {
      return absl::InvalidArgumentError("Empty field name not allowed.");
    }

    if (field_map.contains(field_name)) {
      return absl::InvalidArgumentError(
          absl::StrCat("Two fields have same name: ", field_name));
    }

    const absl::StatusOr<Type> type = Type::Create(type_proto);
    if (!type.ok()) {
      return type.status();
    }

    field_map.insert({field_name, *type});
  }

  return Schema(optional_name, field_map);
}

// Construct result by considering the access paths of all fields. If a field
// has no access paths, consider it a leaf and add the field name as an access
// path. Otherwise, prepend the field name onto all of its type's access paths.
ir::SelectorAccessPathSet Schema::GetAccessPaths() const {
  ir::SelectorAccessPathSet result;
  for (const auto &field_name_type_pair : fields_) {
    std::string const &field_name = field_name_type_pair.first;
    ir::Selector selector = ir::Selector(ir::FieldSelector(field_name));

    const Type &field_type = field_name_type_pair.second;

    ir::SelectorAccessPathSet field_access_paths =
        field_type.GetAccessPaths();
    if (field_access_paths.IsEmpty()) {
      result.Add(ir::SelectorAccessPath(selector));
      continue;
    }
    result.AddAll(
        ir::SelectorAccessPathSet::AddParentToAll(
            std::move(selector), std::move(field_access_paths)));
  }

  return result;
}

}  // namespace raksha::transform::arcs_manifest_tree
