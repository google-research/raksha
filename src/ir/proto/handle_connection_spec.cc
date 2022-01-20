//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

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
  types::Type type = types::proto::Decode(proto.type());
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
