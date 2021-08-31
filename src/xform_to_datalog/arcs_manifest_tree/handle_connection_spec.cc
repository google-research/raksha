#include "src/xform_to_datalog/arcs_manifest_tree/handle_connection_spec.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

namespace types = raksha::ir::types;

HandleConnectionSpec HandleConnectionSpec::CreateFromProto(
    const arcs::HandleConnectionSpecProto &handle_connection_spec_proto) {
  std::string name = handle_connection_spec_proto.name();
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
  CHECK(handle_connection_spec_proto.has_type())
    << "Found connection spec " << name << " without required type.";
  std::unique_ptr<types::Type> type =
      types::Type::CreateFromProto(handle_connection_spec_proto.type());
  return HandleConnectionSpec(std::move(name), reads, writes, std::move(type));
}

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree
