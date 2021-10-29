#ifndef SRC_IR_PROTO_TAG_CLAIM_H_
#define SRC_IR_PROTO_TAG_CLAIM_H_

#include "src/ir/tag_claim.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

  // Create a list of TagClaims from an Assume proto. Each of these will be
  // uninstantiated and thus rooted at the ParticleSpec.
  std::vector<TagClaim> Decode(
    std::string claiming_particle_name,
    const arcs::ClaimProto_Assume &assume_proto);

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_TAG_CLAIM_H_
