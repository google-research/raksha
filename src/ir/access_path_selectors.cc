#include "src/ir/selector.h"

#include <string>

#include "absl/strings/str_join.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

// We create the AccessPathSelectors string by just joining the contents of the
// component vector in reverse, using '.' as a separator.
std::string AccessPathSelectors::ToString() const {
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
