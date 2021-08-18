#ifndef SRC_IR_ACCESS_PATH_SELECTORS_H_
#define SRC_IR_ACCESS_PATH_SELECTORS_H_

#include "src/ir/selector.h"

#include <vector>

#include "absl/hash/hash.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace raksha::ir {

// Represents an AccessPathSelectors descending through the members of types in a
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
// 4. The name AccessPathSelectors is a bit more self-documenting.
class AccessPathSelectors {
 public:
  // Create a leaf AccessPathSelectors from a single leaf selector.
  explicit AccessPathSelectors(Selector leaf) {
    reverse_selectors_.push_back(std::move(leaf));
  }

  // Use the default move constructor for AccessPathSelectors.
  AccessPathSelectors(AccessPathSelectors &&other) = default;
  AccessPathSelectors &operator=(AccessPathSelectors &&other) = default;

  AccessPathSelectors(Selector parent_selector, AccessPathSelectors child_path)
    : AccessPathSelectors(std::move(child_path))
  {
    reverse_selectors_.push_back(std::move(parent_selector));
  }

  // Are two AccessPaths equal. Should be true exactly when they have
  // equivalent string representations.
  bool operator==(const AccessPathSelectors &other) const;

  // Turns this AccessPathSelectors into a string representation chaining
  // together the string representations of the various selectors.
  std::string ToString() const;

  template<typename H>
  friend H AbslHashValue(H h, const AccessPathSelectors &instance) {
    return H::combine(std::move(h), instance.reverse_selectors_);
  }

  // Allow AccessPathSelectors to be copied explicitly.
  AccessPathSelectors Copy() const {
    std::vector<Selector> new_reverse_selector_vec;
    for (const Selector &selector : reverse_selectors_) {
      new_reverse_selector_vec.push_back(selector.Copy());
    }
    return AccessPathSelectors(std::move(new_reverse_selector_vec));
  }

 private:
  explicit AccessPathSelectors(std::vector<Selector> reverse_selectors)
    : reverse_selectors_(std::move(reverse_selectors)) {}

  // We store the path components in reverse order. We do this because we
  // will be creating access paths by recursing down to the bottom of a
  // manifest's tree and then returning up, collecting components as we go.
  // Storing it as a vector in reverse order allows us to continually push
  // the components onto the end of the vector in a single AccessPathSelectors
  // that is moved from callee towards caller.
  std::vector<Selector> reverse_selectors_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_SELECTORS_H_
