#include "src/ir/access_path_selectors.h"

#include "absl/strings/str_split.h"

namespace raksha::ir {

// This is a bit simplified right now, as it only splits on '.', the field
// separator character. However, this could easily be generalized to handle
// other forms of separators later.
AccessPathSelectors AccessPathSelectors::CreateFromString(std::string str) {
  const std::vector<std::string> selector_strs =
      absl::StrSplit(std::move(str), '.', absl::SkipEmpty());

  // Create an empty leaf selector.
  AccessPathSelectors access_path_selectors;

  // Add all selectors as parents, in reverse order of the reversed vector.
  for (
      auto rev_iter = selector_strs.rbegin();
      rev_iter != selector_strs.rend();
      ++rev_iter) {
    access_path_selectors = AccessPathSelectors(
        Selector(FieldSelector(*rev_iter)),
        std::move(access_path_selectors));
  }

  return access_path_selectors;
}

}  // namespace raksha::ir
