#ifndef SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
#define SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_

#include <string>

#include "src/common/logging/logging.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace src::xform_to_datalog::arcs_manifest_tree {

// A class used to gather information about HandleConnectionSpecs from the
// equivalent ArcsManifestProtos. Currently, the only thing we care about
// from these is the name of the handle connection and whether it reads
// and/or writes the handle to which it is connected.
class HandleConnectionSpec {
 public:
  // Factory creating a HandleConnectionSpec from an arcs
  // HandleConnectionSpecProto.
  static HandleConnectionSpec CreateFromProto(
      const arcs::HandleConnectionSpecProto &handle_connection_spec_proto);

  const std::string &name() const {
    return name_;
  }

  bool reads() const {
    return reads_;
  }

  bool writes() const {
    return writes_;
  }

  // Make an arcs HandleConnectionSpecProto from this object.
  arcs::HandleConnectionSpecProto MakeProto() const {
    arcs::HandleConnectionSpecProto result;
    result.set_direction(
        (reads_ && writes_)
        ? arcs::HandleConnectionSpecProto_Direction_READS_WRITES
        : (reads_) ? arcs::HandleConnectionSpecProto_Direction_READS
                   : arcs::HandleConnectionSpecProto_Direction_WRITES);
    result.set_name(name_);
    return result;
  }

 private:
  HandleConnectionSpec(
    const std::string name,
    const bool reads,
    const bool writes) : name_(name), reads_(reads), writes_(writes) {}

  std::string name_;
  bool reads_;
  bool writes_;
};

}  // namespace src::xform_to_datalog::arcs_manifest_tree

#endif  // SRC_XFORM_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
