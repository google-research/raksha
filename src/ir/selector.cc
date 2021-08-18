#include "src/ir/selector.h"

#include "absl/types/variant.h"

namespace raksha::ir {

// A callable struct used to dispatch to the specific selector's
// implementation of ToString.
struct GenericSelectorToStringFunction {
 public:
  template<class T>
  std::string operator()(const T &specific_selector) const {
    return specific_selector.ToString();
  }
};

// Apply the callable ToString struct to the specific_selector_ variant to
// get the result of the specific_selector_'s ToString.
std::string Selector::ToString() const {
  return absl::visit(GenericSelectorToStringFunction(), specific_selector_);
}

// absl::variant has a pre-packaged operator equals that handles the dispatch
// to the specific selector as we would expect.
bool Selector::operator==(const Selector &other) const {
  return specific_selector_ == other.specific_selector_;
}

// A callable struct used to dispatch to the specific selector's
// implementation of Copy.
struct GenericSelectorCopyFunction {
 public:
  template<class T>
  Selector operator()(const T &specific_selector) const {
    return Selector(specific_selector.Copy());
  }
};

Selector Selector::Copy() const {
  return absl::visit(GenericSelectorCopyFunction(), specific_selector_);
}

}  // namespace raksha::ir
