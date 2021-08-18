#include "src/ir/selector.h"

#include <string>

#include "absl/strings/str_join.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

// A helper formatter that just calls ToString on a Selector. Used to make
// AccessPathSelectors::ToString simpler.
struct SelectorToStringFormatter {
  void operator()(std::string *out, const Selector &selector) const {
    out->append(selector.ToString());
  }
};

// We create the AccessPathSelectors string by just joining the contents of the
// component vector in reverse, using '.' as a separator.
std::string AccessPathSelectors::ToString() const {
  return absl::StrJoin(
      reverse_selectors_.rbegin(),
      reverse_selectors_.rend(),
      "",
      SelectorToStringFormatter());
}

}  // namespace raksha::ir
