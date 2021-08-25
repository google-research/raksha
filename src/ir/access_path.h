#ifndef SRC_IR_ACCESS_PATH_H_
#define SRC_IR_ACCESS_PATH_H_

#include "absl/strings/str_cat.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::ir {

// An AccessPath is a fully-qualified path from the root of the data. At this
// time, it is just an AccessPathSelectors, describing the path through the
// type tree, and a root string, describing the path before that. More
// structure may be added to the root in the future.
class AccessPath {
 public:
  explicit AccessPath(
      std::string root_string, AccessPathSelectors access_path_selectors)
      : root_string_(std::move(root_string)),
        access_path_selectors_(std::move(access_path_selectors)) {}

  std::string ToString() const {
    return absl::StrCat(root_string_, access_path_selectors_.ToString());
  }

 private:
  std::string root_string_;
  AccessPathSelectors access_path_selectors_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_H_
