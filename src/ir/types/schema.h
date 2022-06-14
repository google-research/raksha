#ifndef SRC_IR_TYPES_SCHEMA_H_
#define SRC_IR_TYPES_SCHEMA_H_

#include <optional>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/type.h"

namespace raksha::ir::types {

class Schema {
 public:
  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const;

  const std::optional<std::string>& name() const { return name_; }

  const absl::flat_hash_map<std::string, Type>& fields() const {
    return fields_;
  }

  friend class TypeFactory;

  //return a string representing the Schema in the following format: 
  // <schema name> {
  //    <field_name>: <field_kind>
  //    ... 
  // }
  //only showing the first layer of the schema, e.g. not returning the values of <field_kind>
  //for easier testing, fields are sorted alphabetically based on their name  
  std::string ToString() const{
    std::string schema_to_string = absl::StrCat(name_.value_or(""), " {\n"); 
    std::vector<std::string> fields_to_string; 
    for (auto& [name, kind] : fields_) {
      auto kind_to_string = kind.type_base().kind() == raksha::ir::types::TypeBase::Kind::kPrimitive ? "primitive" : "entity"; 
      fields_to_string.push_back(absl::StrCat("\t", name, ": ", kind_to_string));

    //TODO: decide if we want to further print out fields of EntityType (will be a bit more complex given entity includes schema.h)
    }

    //sorting for easier testing 
    sort(fields_to_string.begin(), fields_to_string.end()); 
    absl::StrAppend(&schema_to_string, absl::StrJoin(fields_to_string, "\n"), "\n}"); 
    return schema_to_string;  
  }

 private:
  explicit Schema(std::optional<std::string> name,
                  absl::flat_hash_map<std::string, Type> fields)
      : name_(std::move(name)), fields_(std::move(fields)) {}

  std::optional<std::string> name_;
  absl::flat_hash_map<std::string, Type> fields_;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_SCHEMA_H_
