//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
#ifndef SRC_IR_TYPES_SCHEMA_H_
#define SRC_IR_TYPES_SCHEMA_H_

#include <optional>

#include "absl/container/flat_hash_set.h"
#include "src/common/containers/hash_map.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class Schema {
 public:
  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const;

  const std::optional<std::string>& name() const { return name_; }

  const common::containers::HashMap<std::string, Type>& fields() const {
    return fields_;
  }

  friend class TypeFactory;

  // return a string representing the Schema in the following format:
  //  schema <schema name> {
  //     <field_name>: <field_kind>
  //     ...
  //  }
  // only showing the first layer of the schema, e.g. not returning the values
  // of <field_kind> for easier testing, fields are sorted alphabetically based
  // on their name
  std::string ToString() const {
    std::string schema_name = name_.value_or("");
    std::string schema_to_string = absl::StrCat(
        "schema ", schema_name, schema_name == "" ? "{\n" : " {\n");
    std::vector<std::string> fields_to_string;
    for (const auto& [name, kind] : fields_) {
      auto kind_to_string =
          kind.type_base().kind() ==
                  raksha::ir::types::TypeBase::Kind::kPrimitive
              ? "primitive"
              : "entity";
      fields_to_string.push_back(
          absl::StrCat("  ", name, ": ", kind_to_string));

      // TODO(#585): decide if we want to further print out fields of EntityType
      // (will be a bit more complex given entity_type includes schema.h)
    }

    // Sorting to canonicalize output.
    sort(fields_to_string.begin(), fields_to_string.end());
    absl::StrAppend(&schema_to_string, absl::StrJoin(fields_to_string, "\n"),
                    "\n}");
    return schema_to_string;
  }

 private:
  explicit Schema(std::optional<std::string> name,
                  common::containers::HashMap<std::string, Type> fields)
      : name_(std::move(name)), fields_(std::move(fields)) {}

  std::optional<std::string> name_;
  common::containers::HashMap<std::string, Type> fields_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_SCHEMA_H_
