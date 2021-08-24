#include "src/ir/types/schema.h"

#include "absl/types/optional.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/selector.h"
#include "src/ir/field_selector.h"

namespace raksha::ir::types {

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

}  // namespace raksha::transform::types
