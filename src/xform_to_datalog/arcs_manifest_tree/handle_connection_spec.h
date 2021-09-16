#ifndef SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
#define SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_

#include <string>

#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "src/ir/proto/type.h"
#include "src/ir/types/type.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

// A class used to gather information about HandleConnectionSpecs from the
// equivalent ArcsManifestProtos.
class HandleConnectionSpec {
 public:
  // Factory creating a HandleConnectionSpec from an arcs
  // HandleConnectionSpecProto.
  static HandleConnectionSpec CreateFromProto(
      const arcs::HandleConnectionSpecProto &handle_connection_spec_proto);

  const std::string &name() const { return name_; }

  bool reads() const { return reads_; }

  bool writes() const { return writes_; }

  const raksha::ir::types::Type &type() const { return *type_; }

  // Make an arcs HandleConnectionSpecProto from this object.
  arcs::HandleConnectionSpecProto MakeProto() const {
    arcs::HandleConnectionSpecProto result;
    result.set_direction(
        (reads_ && writes_)
        ? arcs::HandleConnectionSpecProto_Direction_READS_WRITES
        : (reads_) ? arcs::HandleConnectionSpecProto_Direction_READS
                   : arcs::HandleConnectionSpecProto_Direction_WRITES);
    result.set_name(name_);
    *result.mutable_type() = raksha::ir::types::proto::encode(*type_);
    return result;
  }

  // Get all of the AccessPaths rooted at the associated ParticleSpec
  // (indicated through the provided name) through leaf fields of type_.
  std::vector<raksha::ir::AccessPath> GetAccessPaths(
      std::string particle_spec_name) const {
    namespace ir = raksha::ir;
    std::vector<ir::AccessPath> result_paths;
    ir::AccessPathSelectorsSet selectors_set =
        type_->GetAccessPathSelectorsSet();
    ir::AccessPathRoot root(
        ir::HandleConnectionSpecAccessPathRoot(
            std::move(particle_spec_name), name_));
    for (ir::AccessPathSelectors selectors :
         ir::AccessPathSelectorsSet::CreateAbslSet(selectors_set)) {
      result_paths.push_back(ir::AccessPath(root, std::move(selectors)));
    }
    return result_paths;
  }

 private:
  HandleConnectionSpec(
    std::string name, bool reads, bool writes,
    std::unique_ptr<raksha::ir::types::Type> type)
    : name_(std::move(name)), reads_(reads), writes_(writes),
      type_(std::move(type)) {}

  // The name of this HandleConnectionSpec.
  std::string name_;
  // Indicates whether the associated ParticleSpec reads this
  // HandleConnectionSpec.
  bool reads_;
  // Indicates whether the associated ParticleSpec writes this
  // HandleConnectionSpec.
  bool writes_;
  // The type of this HandleConnectionSpec.
  std::unique_ptr<raksha::ir::types::Type> type_;
};

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree

#endif  // SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
