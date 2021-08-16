#include "src/ir/field_selector.h"

#include "absl/strings/str_cat.h"

#include <string>

namespace raksha::ir {

std::string FieldSelector::ToString() const {
  return absl::StrCat(".", field_name_);
}

}  // namespace raksha::ir
