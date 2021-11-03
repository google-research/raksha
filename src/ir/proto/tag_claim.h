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
