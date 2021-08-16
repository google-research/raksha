#ifndef SRC_IR_SELECTOR_H_
#define SRC_IR_SELECTOR_H_

#include "field_selector.h"

#include "absl/types/variant.h"

#include <string>

namespace raksha::ir {

class Selector {
 public:
  explicit Selector(const FieldSelector field_selector)
    : specific_selector_(field_selector) {}

  std::string ToString() const;

  bool operator==(const Selector &other) const;

  template<typename H>
  friend H AbslHashValue(H h, const Selector &selector) {
    return H::combine(std::move(h), selector.specific_selector_);
  }


 private:
  absl::variant<FieldSelector> specific_selector_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SELECTOR_H_
