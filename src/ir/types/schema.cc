//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------
#include "src/ir/types/schema.h"

#include <optional>

#include "src/common/logging/logging.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/selector.h"
#include "src/ir/field_selector.h"

namespace raksha::ir::types {

namespace ir = raksha::ir;

// Construct result by considering the access paths of all fields. If a field
// has no access paths, consider it a leaf and add the field name as an access
// path. Otherwise, prepend the field name onto all of its type's access paths.
ir::AccessPathSelectorsSet Schema::GetAccessPathSelectorsSet() const {
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
        field_name_type_pair.second.GetAccessPathSelectorsSet();

    result = ir::AccessPathSelectorsSet::Union(
        std::move(result),
        ir::AccessPathSelectorsSet::AddParentToAll(
            std::move(selector), std::move(field_access_paths)));
  }

  return result;
}

}  // namespace raksha::transform::types
