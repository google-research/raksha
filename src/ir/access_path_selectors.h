#ifndef SRC_IR_ACCESS_PATH_SELECTORS_H_
#define SRC_IR_ACCESS_PATH_SELECTORS_H_

#include "src/ir/selector.h"

#include <vector>

#include "absl/hash/hash.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"

namespace raksha::ir {

// Represents an AccessPathSelectors descending through the members of types in
// a. This is eventually used for printing edges to the resultant Datalog file.
//
// We use this class instead of a bare vector for the following reasons:
//
// 1. Encapsulating the reversed vector prevents us from having to reason
// about this all the time and potentially be confused by it.
//
// 2. Providing our own interface allows us to make this class immutable
// except for move constructors. We define no "add", "insert", etc. methods;
// "mutation" is done only by constructing a new object from a previous
// object. That prevents many forms of state-manipulation bugs.
//
// 3. The name AccessPathSelectors is a bit more self-documenting.
class AccessPathSelectors {
 public:

  // Given a string representation of an AccessPathSelectors object, return
  // the equivalent structured representation.
  static AccessPathSelectors CreateFromString(std::string str);

  // Default empty constructor creates an access path with no Selectors. This
  // would correspond to a type tree consisting of a primitive type at the
  // root.
  AccessPathSelectors() = default;

  // Create a leaf AccessPathSelectors from a single leaf selector.
  explicit AccessPathSelectors(Selector leaf) {
    reverse_selectors_.push_back(std::move(leaf));
  }

  // Create a new AccessPathSelectors object from a Selector and an
  // AccessPathSelectors object, with the Selector added as the parent of the
  // path elements of the AccessPathSelectors object.
  AccessPathSelectors(Selector parent_selector, AccessPathSelectors child_path)
    : AccessPathSelectors(std::move(child_path))
  {
    reverse_selectors_.push_back(std::move(parent_selector));
  }

  // Are two AccessPathSelectors equal. This is the same as their component
  // vectors being equal according to std::vector's operator==.
  bool operator==(const AccessPathSelectors &other) const {
    return reverse_selectors_ == other.reverse_selectors_;
  }

  // Turns this AccessPathSelectors into a string representation chaining
  // together the string representations of the various selectors. Just
  // concatenates together the string representation of all Selectors from
  // parent selector to child selector (which is the reverse order of how
  // they are stored internally).
  std::string ToString() const {
      return absl::StrJoin(
      reverse_selectors_.rbegin(),
      reverse_selectors_.rend(),
      "",
      [](std::string *out, const Selector &selector){
        out->append(selector.ToString()); });

  }

  template<typename H>
  friend H AbslHashValue(H h, const AccessPathSelectors &instance) {
    return H::combine(std::move(h), instance.reverse_selectors_);
  }

 private:
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
