#include "particle_spec.h"

#include "handle_connection_spec.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

absl::StatusOr<ParticleSpec> ParticleSpec::Create(
    const arcs::ParticleSpecProto &particle_spec_proto) {
  const std::string name = particle_spec_proto.name();
  if (name.empty()) {
    return absl::InvalidArgumentError("Missing name for ParticleSpec.");
  }

  absl::flat_hash_map<std::string, HandleConnectionSpec> connections;
  for (const arcs::HandleConnectionSpecProto &connection_proto :
    particle_spec_proto.connections()) {
    absl::StatusOr<HandleConnectionSpec> connection =
        HandleConnectionSpec::Create(connection_proto);
    if (!connection.ok()) {
      return connection.status();
    }

    const std::string connection_name = connection->name();
    auto insert_result = connections.insert({connection_name, *connection});
    const bool insert_successful = insert_result.second;
    if (!insert_successful) {
      return absl::InvalidArgumentError(
          absl::StrCat("Duplicate connection name ", connection_name));
    }
  }

}

}
