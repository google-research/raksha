#ifndef SRC_IR_ACCESS_PATH_H_
#define SRC_IR_ACCESS_PATH_H_

#include "absl/strings/str_cat.h"
#include "absl/container/flat_hash_map.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_root.h"

namespace raksha::ir {

// An AccessPath is a fully-qualified path from the root of the data. At this
// time, it is an AccessPathSelectors, describing the path through the type
// tree, and an AccessPathRoot, describing the path before that.
class AccessPath {
 public:
  explicit AccessPath(
    AccessPathRoot root, AccessPathSelectors access_path_selectors)
    : root_(std::move(root)),
      access_path_selectors_(std::move(access_path_selectors)) {}

  // Express the AccessPath as a string. This is accomplished by
  // concatenating the root string and the selectors string.
  std::string ToString() const {
    return absl::StrCat(root_.ToString(), access_path_selectors_.ToString());
  }

  // Express the AccessPath as a string. This is accomplished by
  // concatenating the root string and the selectors string.
  std::string ToDatalog(const DatalogPrintContext &ctxt) const {
    return absl::StrCat(root_.ToDatalog(ctxt),
                        access_path_selectors_.ToString());
  }

  // Return a new AccessPath, identical to *this, except with the AccessPath
  // root replaced with the indicated instantiated root. Note that this expects
  // the current access path to not already be instantiated.
  AccessPath Instantiate(AccessPathRoot new_root) const {
    CHECK(!root_.IsInstantiated())
      << "Attempt to instantiate an AccessPath that is already instantiated.";
    CHECK(new_root.IsInstantiated())
      << "Attempt to instantiate an AccessPath with an uninstantiated root.";
    return AccessPath(std::move(new_root), access_path_selectors_);
  }

  // This is used to allow this AccessPath to participate in a bulk
  // instantiation of AccessPaths (such as, for instance, all of the
  // AccessPaths referenced by a particular ParticleSpec).
  AccessPath BulkInstantiate(
      const absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
      &instantiation_map) const {
    // Check that the root is not already instantiated. This may not be
    // the correct long-term behavior; we may want to just returning the
    // current instantiated AccessPath. Asserting it is not instantiated,
    // however, is safe and easy short-term behavior.
    CHECK(!root_.IsInstantiated())
      << "Expected to instantiate only uninstantiated roots.";
    auto find_res = instantiation_map.find(root_);
    CHECK(find_res != instantiation_map.end())
      << "Could not find entry to instantiate access path in "
      << "instantiation_map.";
    AccessPathRoot new_root = find_res->second;
    return Instantiate(std::move(new_root));
  }

  const AccessPathRoot &root() const { return root_; }

  const AccessPathSelectors &selectors() const {
    return access_path_selectors_;
  }

  bool operator==(const AccessPath &other) const {
    return (root_ == other.root_) &&
      (access_path_selectors_ == other.access_path_selectors_);
  }

  template<typename H>
  friend H AbslHashValue(H h, const AccessPath &access_path) {
    return H::combine(
        std::move(h), access_path.root_, access_path.access_path_selectors_);
  }

 private:
  AccessPathRoot root_;
  AccessPathSelectors access_path_selectors_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_H_
