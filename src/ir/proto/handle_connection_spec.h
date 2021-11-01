#ifndef SRC_IR_PROTO_HANDLE_CONNECTION_SPEC_H_
#define SRC_IR_PROTO_HANDLE_CONNECTION_SPEC_H_

#include "src/ir/handle_connection_spec.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

HandleConnectionSpec Decode(const arcs::HandleConnectionSpecProto &proto);

arcs::HandleConnectionSpecProto Encode(const HandleConnectionSpec &hcs);

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_HANDLE_CONNECTION_SPEC_H_
