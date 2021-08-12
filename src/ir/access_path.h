#ifndef SRC_IR_ACCESS_PATH_H_
#define SRC_IR_ACCESS_PATH_H_

#include "absl/strings/string_view.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include <vector>

namespace raksha::ir {

// Represents an AccessPath descending through Manifest substructures. This
// is eventually used for printing edges to the resultant Datalog file. It
// does not own any strings to which it refers; it relies upon the original
// Manifest tree structures generated from the original protos outliving it.
//
// Currently, this does not add type annotations onto the individual
// components of the AccessPath. That may be wise to do in the future.
class AccessPath {
 public:
  // Are two AccessPaths equal. Should be true exactly when they have
  // equivalent string representations.
  bool operator==(const AccessPath &other) const;

  // Turns this AccessPath into a string representation. A path to the field
  // f in the type T in particle P in recipe R will produce the string:
  //
  // "R.P.T.f"
  std::string ToString() const;

  // Create a leaf AccessPath from a string indicating a single string
  // component. This will return an error status if
  // check_component_allowed(leaf_component) does not return Status::Ok.
  static absl::StatusOr<AccessPath> Create(absl::string_view leaf_component);

  // Create a new AccessPath that is a parent of the parameter access_path by
  // adding parent_component directly above the contents of access_path. If
  // the string representation of access path is "a.b.c" and parent_component
  // is "q", then the resulting access path would have the representation:
  //
  // "q.a.b.c"
  //
  // This will return an error status if
  // check_component_allowed(parent_component) does not return Status::Ok.
  static absl::StatusOr<AccessPath> CreateParent(
      absl::string_view parent_component,
      AccessPath access_path);

 private:
  // Create an access path having a single (leaf) component.
  explicit AccessPath(const absl::string_view leaf_component)
    : reverse_components_{ leaf_component }
  {}

  // Checks that the passed-in component is legal. Currently this means that
  // it is not empty and does not contain a dot separator.
  static absl::Status CheckComponentAllowed(absl::string_view component);

  // We store the path components in reverse order. We do this because we
  // will be creating access paths by recursing down to the bottom of a
  // manifest's tree and then returning up, collecting components as we go.
  // Storing it as a vector in reverse order allows us to continually push
  // the components onto the end of the vector in a single AccessPath that is
  // moved from callee towards caller.
  std::vector<absl::string_view> reverse_components_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_H_
