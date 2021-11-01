#ifndef SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
#define SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_

#include <string>

#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "src/ir/types/type.h"

namespace raksha::ir {

// A class used to gather information about HandleConnectionSpecs from the
// equivalent ArcsManifestProtos.
class HandleConnectionSpec {
 public:
  explicit HandleConnectionSpec(
    std::string name, bool reads, bool writes,
    std::unique_ptr<types::Type> type)
    : name_(std::move(name)), reads_(reads), writes_(writes),
      type_(std::move(type)) {}

  const std::string &name() const { return name_; }

  bool reads() const { return reads_; }

  bool writes() const { return writes_; }

  const types::Type &type() const { return *type_; }

  // Get all of the AccessPaths rooted at the associated ParticleSpec
  // (indicated through the provided name) through leaf fields of type_.
  std::vector<AccessPath> GetAccessPaths(
      std::string particle_spec_name) const {
    std::vector<AccessPath> result_paths;
    AccessPathSelectorsSet selectors_set =
        type_->GetAccessPathSelectorsSet();
    AccessPathRoot root(
        HandleConnectionSpecAccessPathRoot(
            std::move(particle_spec_name), name_));
    for (AccessPathSelectors selectors :
         AccessPathSelectorsSet::CreateAbslSet(selectors_set)) {
      result_paths.push_back(AccessPath(root, std::move(selectors)));
    }
    return result_paths;
  }

 private:
  // The name of this HandleConnectionSpec.
  std::string name_;
  // Indicates whether the associated ParticleSpec reads this
  // HandleConnectionSpec.
  bool reads_;
  // Indicates whether the associated ParticleSpec writes this
  // HandleConnectionSpec.
  bool writes_;
  // The type of this HandleConnectionSpec.
  std::unique_ptr<types::Type> type_;
};

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree

#endif  // SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
