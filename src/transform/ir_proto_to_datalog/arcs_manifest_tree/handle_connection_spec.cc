#include "handle_connection_spec.h"
#include "type.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

#include <iostream>

namespace raksha::transform::arcs_manifest_tree {

absl::StatusOr<HandleConnectionSpec> HandleConnectionSpec::Create(
  const arcs::HandleConnectionSpecProto &handle_connection_spec_proto) {
  std::string const &connection_spec_name = handle_connection_spec_proto.name();
  if (connection_spec_name.empty()) {
    return absl::InvalidArgumentError("Missing name in HandleConnectionSpec.");
  }

  // We expect to fill both of these with true or false, but if we were to
  // mess up somehow, setting them to true is the more conservative direction.
  const auto direction = handle_connection_spec_proto.direction();

  if (direction == arcs::HandleConnectionSpecProto_Direction_UNSPECIFIED) {
    return absl::InvalidArgumentError(
        absl::StrCat(
            "Unspecified direction in HandleConnectionSpec ",
            connection_spec_name));
  }

  const bool reads =
    (direction == arcs::HandleConnectionSpecProto_Direction_READS)
      || (direction == arcs::HandleConnectionSpecProto_Direction_READS_WRITES);

  const bool writes =
    (direction == arcs::HandleConnectionSpecProto_Direction_WRITES)
      || (direction == arcs::HandleConnectionSpecProto_Direction_READS_WRITES);


  if (!handle_connection_spec_proto.has_type()) {
    return absl::InvalidArgumentError(
        "Unspecified type in HandleConnectionSpec.");
  }
  const absl::StatusOr<Type> type =
      Type::Create(handle_connection_spec_proto.type());
  if (!type.ok()) {
    return type.status();
  }

  return HandleConnectionSpec(connection_spec_name, reads, writes, *type);
}

}  // namespace arcs_manifest_tree
