#ifndef SRC_IR_ACCESS_PATH_H_
#define SRC_IR_ACCESS_PATH_H_

#include "absl/strings/str_cat.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_root.h"

namespace raksha::ir {

// An AccessPath is a fully-qualified path from the root of the data. At this
// time, it is an AccessPathSelectors, describing the path through the type
// tree, and an AccessPathRoot, describing the path before that.
class AccessPath {
 public:
  static AccessPath CreateSpecAccessPath(
      AccessPathSelectors access_path_selectors) {
    return AccessPath(
        AccessPathRoot(SpecAccessPathRoot()), std::move(access_path_selectors));
  }

  // Create an AccessPath from the equivalent Arcs proto. We currently only
  // handle the case in which an AccessPathProto is rooted at a
  // (particle_spec, handle_connection), in which case it
  static AccessPath CreateFromProto(
      const arcs::AccessPathProto &access_path_proto) {
    CHECK(!access_path_proto.has_store_id())
      << "Currently, access paths involving stores are not implemented.";
    AccessPathSelectors selectors =
        AccessPathSelectors::CreateFromProto(access_path_proto);
    return CreateSpecAccessPath(selectors);
  }

  // Return a new AccessPath, identical to *this, except with the AccessPath
  // root replaced with the indicated concrete root. Note that this expects the
  // passed-in spec_access_path to not already be instantiated.
  AccessPath Instantiate(ConcreteAccessPathRoot new_root) const {
    CHECK(!root_.IsInstantiated())
      << "Attempt to instantiate an AccessPath that is already instantiated.";
    return AccessPath(
        AccessPathRoot(std::move(new_root)), access_path_selectors_);
  }

  explicit AccessPath(
    AccessPathRoot root, AccessPathSelectors access_path_selectors)
    : root_(std::move(root)),
      access_path_selectors_(std::move(access_path_selectors)) {}

  // Express the AccessPath as a string. This is accomplished by
  // concatenating the root string and the selectors string.
  std::string ToString() const {
    return absl::StrCat(root_.ToString(), access_path_selectors_.ToString());
  }

 private:

  AccessPathRoot root_;
  AccessPathSelectors access_path_selectors_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_H_
