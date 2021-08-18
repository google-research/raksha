#ifndef SRC_IR_FIELD_SELECTOR_H_
#define SRC_IR_FIELD_SELECTOR_H_

#include <string>

#include "absl/strings/str_cat.h"

namespace raksha::ir {

// Represents descending through the type tree by taking the field of an
// EntityType in an AccessPath.
class FieldSelector {
 public:
  FieldSelector(std::string field_name) : field_name_(std::move(field_name)) {}

  // Prints the string representing dereferencing the field in the AccessPath.
  // This will just be the "." punctuation plus the name of the field.
  std::string ToString() const {
    return absl::StrCat(".", field_name_);
  }

  // Two fields selectors are equal exactly when their names are equal.
  bool operator==(const FieldSelector &other) const {
    return field_name_ == other.field_name_;
  }

  template<typename H>
  friend H AbslHashValue(H h, const FieldSelector &field_selector) {
    return H::combine(std::move(h), field_selector.field_name_);
  }

 private:
  // All of the specialness of a FieldSelector is contained within its name.
  std::string field_name_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_FIELD_SELECTOR_H_
