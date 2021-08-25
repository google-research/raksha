#include "src/xform_to_datalog/arcs_manifest_tree/handle_connection_spec.h"

namespace src::xform_to_datalog::arcs_manifest_tree {

HandleConnectionSpec HandleConnectionSpec::CreateFromProto(
    const arcs::HandleConnectionSpecProto &handle_connection_spec_proto) {
  const std::string &name = handle_connection_spec_proto.name();
  CHECK(!name.empty()) << "Found connection spec without required name.";
  bool reads = false;
  bool writes = false;
  switch (handle_connection_spec_proto.direction()) {
    case arcs::HandleConnectionSpecProto_Direction_UNSPECIFIED:
      LOG(FATAL)
        << "Connection spec " << name << " has unspecified direction";
    case arcs::HandleConnectionSpecProto_Direction_READS:
      reads = true;
      break;
    case arcs::HandleConnectionSpecProto_Direction_WRITES:
      writes = true;
      break;
    case arcs::HandleConnectionSpecProto_Direction_READS_WRITES:
      reads = true;
      writes = true;
      break;
    default:
      LOG(FATAL)
        << "Unimplemented direction in connection spec " << name << ".";
  }
  return HandleConnectionSpec(name, reads, writes);
}

}  // namespace src::xform_to_datalog::arcs_manifest_tree
