#ifndef SRC_IR_SELECTOR_ACCESS_PATH_H_
#define SRC_IR_SELECTOR_ACCESS_PATH_H_

#include "selector.h"

#include "absl/hash/hash.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include <vector>

namespace raksha::ir {

// Represents an SelectorAccessPath descending through the members of types in a
// Manifest. This is eventually used for printing edges to the resultant Datalog
// file.
//
// We use this class instead of a bare vector for the following reasons:
//
// 1. Encapsulating the reversed vector prevents us from having to reason
// about this all the time and potentially be confused by it.
//
// 2. Adding our own containing struct allows us to force copies to be
// explicitly requested by making the copy constructor private, but
// accessible with a public "copy" function. Because we are returning this
// object owning variable-length memory in a bottom-up fashion, copies could
// cause us to have quadratic performance.
//
// 3. Providing our own interface allows us to make this class immutable
// except for move constructors. We define no "add", "insert", etc. methods;
// "mutation" is done only by constructing a new object from a previous
// object. That prevents many forms of state-manipulation bugs.
//
// 4. The name SelectorAccessPath is a bit more self-documenting.
class SelectorAccessPath {
 public:
  // Create a leaf SelectorAccessPath from a single leaf selector.
  explicit SelectorAccessPath(const Selector leaf) : reverse_selectors_{leaf} {}

  // Use the default move constructor for SelectorAccessPath.
  SelectorAccessPath(SelectorAccessPath &&other) = default;
  SelectorAccessPath &operator=(SelectorAccessPath &&other) = default;

  SelectorAccessPath(Selector parent_selector, SelectorAccessPath child_path)
    : SelectorAccessPath(std::move(child_path))
  {
    reverse_selectors_.push_back(std::move(parent_selector));
  }

  // Are two AccessPaths equal. Should be true exactly when they have
  // equivalent string representations.
  bool operator==(const SelectorAccessPath &other) const;

  // Turns this SelectorAccessPath into a string representation chaining
  // together the string representations of the various selectors.
  std::string ToString() const;

  template<typename H>
  friend H AbslHashValue(H h, const SelectorAccessPath &instance) {
    return H::combine(std::move(h), instance.reverse_selectors_);
  }

  // Allow SelectorAccessPath to be copied explicitly.
  SelectorAccessPath Copy() const {
    return SelectorAccessPath(*this);
  }

 private:
  // Copying AccessPaths unintentionally could end up with superlinear
  // performance rather quickly. Make sure we know when we are doing that by
  // forcing an explicit copy.
  explicit SelectorAccessPath(const SelectorAccessPath &other) = default;

  // We store the path components in reverse order. We do this because we
  // will be creating access paths by recursing down to the bottom of a
  // manifest's tree and then returning up, collecting components as we go.
  // Storing it as a vector in reverse order allows us to continually push
  // the components onto the end of the vector in a single SelectorAccessPath
  // that is moved from callee towards caller.
  std::vector<Selector> reverse_selectors_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SELECTOR_ACCESS_PATH_H_
