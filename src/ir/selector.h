#ifndef SRC_IR_SELECTOR_H_
#define SRC_IR_SELECTOR_H_

#include "field_selector.h"

#include "absl/types/variant.h"

#include <string>

namespace raksha::ir {

// An individual selector in an AccessPath descending down the type tree.
// This Selector is generic, and may contain any method of descending, such
// as field access, array access, tuple element access, etc.
//
// For now, however, we only have implemented the FieldSelector portion.
class Selector {
 public:
  explicit Selector(const FieldSelector field_selector)
    : specific_selector_(field_selector) {}

  // Print the string representing a selector's participation in the
  // AccessPath. This will include the punctuation indicating the method of
  // selection (such as . for string, [ for index, etc) and the string
  // contents of the selector itself.
  std::string ToString() const;

  // Whether two selectors are equal. Will be true if they are the same type
  // of selector and those two types also compare equal.
  bool operator==(const Selector &other) const;

  template<typename H>
  friend H AbslHashValue(H h, const Selector &selector) {
    return H::combine(std::move(h), selector.specific_selector_);
  }

 private:
  // The variant storing the specific type of selector. We will dispatch
  // through this to perform all methods on the generic Selector.
  absl::variant<FieldSelector> specific_selector_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SELECTOR_H_
