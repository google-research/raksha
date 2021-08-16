#include "src/ir/selector_access_path.h"
#include "selector.h"

#include "absl/strings/str_join.h"

#include <string>

namespace raksha::ir {

// Check the components of the two access paths for equality. This is the
// same as their component vectors being equal according to std::vector's
// operator==.
bool SelectorAccessPath::operator==(const SelectorAccessPath &other) const {
  return reverse_selectors_ == other.reverse_selectors_;
}

// We create the SelectorAccessPath string by just joining the contents of the
// component vector in reverse, using '.' as a separator.
std::string SelectorAccessPath::ToString() const {
  std::vector<std::string> reverse_selector_strs;
  for (const Selector &selector : reverse_selectors_) {
    reverse_selector_strs.push_back(selector.ToString());
  }
  return absl::StrJoin(
      reverse_selector_strs.rbegin(),
      reverse_selector_strs.rend(),
      "");
}

}  // namespace raksha::ir
