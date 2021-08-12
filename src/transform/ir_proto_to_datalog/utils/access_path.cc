#include "access_path.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "absl/strings/string_view.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_cat.h"

#include <string>

namespace raksha::transform::utils {

// Check the components of the two access paths for equality. First, ensure
// that they have the same number of components, then check component by
// component.
bool AccessPath::operator==(const AccessPath &other) const {
  if (reverse_components_.size() != other.reverse_components_.size()) {
    return false;
  }

  for (uint64_t idx = 0; idx < reverse_components_.size(); ++idx) {
    if (reverse_components_.at(idx) != other.reverse_components_.at(idx)) {
      return false;
    }
  }

  return true;
}

// We create the AccessPath string by just joining the contents of the
// component vector in reverse, using '.' as a separator.
std::string AccessPath::ToString() const {
  return absl::StrJoin(
      reverse_components_.rbegin(),
      reverse_components_.rend(),
      ".");
}

// Check that the component is nonempty and does not contain a '.'. Both of
// these cases seem malformed and may indicate confusion in deserializing the
// Manifest protos.
absl::Status AccessPath::CheckComponentAllowed(absl::string_view component) {
  if (component.empty()) {
    return absl::InvalidArgumentError(
        "Empty component name not allowed in AccessPath.");
  }

  if (absl::StrContains(component, '.')) {
    const std::string error_message =
        absl::StrCat(
        "Access path component: ",
        component,
        " contains a '.' and thus is compound; only simple component names "
        "are allowed.");
    return absl::InvalidArgumentError(error_message);
  }
  return absl::OkStatus();
}

// Check that the component is allowed, and if so, create a new AccessPath
// having it as a leaf component.
absl::StatusOr<AccessPath> AccessPath::Create(
    const absl::string_view component) {
  const absl::Status component_allowed_status =
      CheckComponentAllowed(component);
  if (!component_allowed_status.ok()) {
    return component_allowed_status;
  }

  return AccessPath(component);
}

// Create a parent access path by creating a copy with the parent_component
// added above all components of the passed-in access_path. Although this
// makes a copy, it is built with copy-elision in mind; it is expected that
// the passed-in access_path will usually no longer be used after being
// passed into this factory. If that is the case, then add_parent_component
// will just push the new component onto the end of the existing access_path,
// making this very efficient.
absl::StatusOr<AccessPath> AccessPath::CreateParent(
    const absl::string_view parent_component,
    AccessPath access_path)
{
  const absl::Status component_allowed_status =
      CheckComponentAllowed(parent_component);
  if (!component_allowed_status.ok()) {
    return component_allowed_status;
  }

  access_path.reverse_components_.push_back(parent_component);
  return access_path;
}


}  // namespace raksha::transform::utils
