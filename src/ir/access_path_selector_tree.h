#ifndef SRC_IR_ACCESS_PATH_SELECTOR_TREE_H_
#define SRC_IR_ACCESS_PATH_SELECTOR_TREE_H_

#include <memory>

#include "src/common/logging/logging.h"
#include "src/ir/selector.h"

namespace raksha::ir {

// A tree of AccessPathSelectors. Used to describe the AccessPaths that
// derive from a particular type. This contains both links to children and
// links to parents to support different use cases: the upwards links so that
// the selectors list of an AccessPath can be expressed as a link to an inner
// node, usually a leaf.
class AccessPathSelectorTree {
 public:
  AccessPathSelectorTree(
      Selector selector,
      std::vector<std::unique_ptr<AccessPathSelectorTree>> children)
      : selector_(std::move(selector)),
        children_(std::move(children)) {}

  void SetParent(const AccessPathSelectorTree &parent) {
    CHECK(parent_ == nullptr)
      << "Attempt to set a parent pointer that has already been set in "
      << "AccessPathSelectorTree.";
    parent_ = &parent;
  }

  // Indicates whether the access paths indicated by the parent pointers are
  // equal for these two AccessPathSelectorTrees.
  bool ParentPathsEqual(const AccessPathSelectorTree &other) const {
    if (selector_ != other.selector_) return false;
    if ((parent_ == nullptr) != (other.parent_ == nullptr)) return false;
    if (parent_ == nullptr) {
      CHECK(other.parent_ == nullptr)
        << "Assumed both parent pointers would be nullptr at this point!";
      return true;
    }
    return parent_->ParentPathsEqual(*other.parent_);
  }

  void PrintParentPath(std::ostream &out) const {
    if (parent_ != nullptr) parent_->PrintParentPath(out);
    out << selector_.ToString();
  }

  class AccessPathSelectorTreeIter {
   private:
    std::vector<const AccessPathSelectorTree *> path_stack_;
  };

 private:
  Selector selector_;
  const AccessPathSelectorTree *parent_;
  std::vector<std::unique_ptr<AccessPathSelectorTree>> children_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_SELECTOR_TREE_H_
