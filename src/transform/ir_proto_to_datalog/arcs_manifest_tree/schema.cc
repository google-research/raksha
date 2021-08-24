#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/schema.h"

#include "absl/types/optional.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/selector.h"
#include "src/ir/field_selector.h"

namespace raksha::transform::arcs_manifest_tree {

namespace ir = raksha::ir;

// Construct result by considering the access paths of all fields. If a field
// has no access paths, consider it a leaf and add the field name as an access
// path. Otherwise, prepend the field name onto all of its type's access paths.
ir::AccessPathSelectorsSet Schema::GetAccessPaths() const {
  ir::AccessPathSelectorsSet result;
  // If the entity type has no fields, we should still generate one empty
  // access path indicating this leaf schema.
  if (fields_.empty()) {
    return ir::AccessPathSelectorsSet({ir::AccessPathSelectors()});
  }
  for (auto &field_name_type_pair : fields_) {
    ir::Selector selector =
        ir::Selector(ir::FieldSelector(field_name_type_pair.first));

    ir::AccessPathSelectorsSet field_access_paths =
        field_name_type_pair.second->GetAccessPaths();

    result = ir::AccessPathSelectorsSet::Union(
        std::move(result),
        ir::AccessPathSelectorsSet::AddParentToAll(
            std::move(selector), std::move(field_access_paths)));
  }

  return result;
}

Schema Schema::CreateFromProto(arcs::SchemaProto schema_proto) {
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

    field_map.insert({field_name, Type::CreateFromProto(type_proto)});
  }

  return Schema(std::move(name), std::move(field_map));
}

arcs::SchemaProto Schema::MakeProto() const {
  arcs::SchemaProto schema_proto;
  if (name_) {
    schema_proto.add_names(*name_);
  }
  auto &fields_map = *schema_proto.mutable_fields();
  for (auto &field_name_type_pair: fields_) {
    const std::string &field_name = field_name_type_pair.first;
    const Type &field_type = *field_name_type_pair.second;

    auto insert_result = fields_map.insert(
        {field_name, field_type.MakeProto()});
    CHECK(insert_result.second)
      << "Found duplicate for field name " << field_name;
  }
  return schema_proto;
}

}  // namespace raksha::transform::arcs_manifest_tree
