#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_PARTICLE_SPEC_H_

#include "handle_connection_spec.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

namespace raksha::transform::arcs_manifest_tree {

class ParticleSpec {
 public:
  absl::StatusOr<ParticleSpec> Create(
      const arcs::ParticleSpecProto &particle_spec_proto);

 private:
  ParticleSpec(
      const std::string name,
      const absl::flat_hash_map<std::string, HandleConnectionSpec>
          connection_specs) : name_(name), connection_specs_(connection_specs)
  {}

  std::string name_;
  absl::flat_hash_map<std::string, HandleConnectionSpec> connection_specs_;
};

} // namespace raksha::transform::arcs_manifest_tree

#endif // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_MANIFEST_H_
