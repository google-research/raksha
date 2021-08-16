#include "src/ir/selector.h"

#include "absl/types/variant.h"

namespace raksha::ir {

struct GenericSelectorToStringFunction {
 public:
  template<class T>
  std::string operator()(const T &specific_selector) const {
    return specific_selector.ToString();
  }
};

std::string Selector::ToString() const {
  return absl::visit(GenericSelectorToStringFunction(), specific_selector_);
}

bool Selector::operator==(const Selector &other) const {
  return specific_selector_ == other.specific_selector_;
}

}  // namespace raksha::ir
