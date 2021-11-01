#include "handle_connection_spec.h"

#include "src/ir/proto/type.h"

namespace raksha::ir::proto {

HandleConnectionSpec Decode(const arcs::HandleConnectionSpecProto &proto) {
  std::string name = proto.name();
  CHECK(!name.empty()) << "Found connection spec without required name.";
  bool reads = false;
  bool writes = false;
  switch (proto.direction()) {
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
  CHECK(proto.has_type())
    << "Found connection spec " << name << " without required type.";
  std::unique_ptr<types::Type> type = types::proto::Decode(proto.type());
  return HandleConnectionSpec(std::move(name), reads, writes, std::move(type));
}

arcs::HandleConnectionSpecProto Encode(const HandleConnectionSpec &hcs) {
    arcs::HandleConnectionSpecProto result;
    result.set_direction(
        (hcs.reads() && hcs.writes())
        ? arcs::HandleConnectionSpecProto_Direction_READS_WRITES
        : (hcs.reads()) ? arcs::HandleConnectionSpecProto_Direction_READS
                   : arcs::HandleConnectionSpecProto_Direction_WRITES);
    result.set_name(hcs.name());
    *result.mutable_type() = types::proto::Encode(hcs.type());
    return result;
}

}  // namespace raksha::ir::proto
