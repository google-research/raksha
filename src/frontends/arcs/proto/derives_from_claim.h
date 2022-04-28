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

#ifndef SRC_FRONTENDS_ARCS_PROTO_DERIVES_FROM_CLAIM_H_
#define SRC_FRONTENDS_ARCS_PROTO_DERIVES_FROM_CLAIM_H_

#include "src/frontends/arcs/derives_from_claim.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

DerivesFromClaim Decode(const ::arcs::ClaimProto_DerivesFrom &proto) {
  CHECK(proto.has_source())
      << "DerivesFrom proto does not have required field source.";
  CHECK(proto.has_target())
      << "DerivesFrom proto does not have required field target.";
  return DerivesFromClaim(proto::Decode(proto.target()),
                          proto::Decode(proto.source()));
}

}  // namespace raksha::frontends::arcs::proto

#endif  // SRC_FRONTENDS_ARCS_PROTO_DERIVES_FROM_CLAIM_H_
