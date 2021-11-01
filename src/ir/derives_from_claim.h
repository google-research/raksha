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

#ifndef SRC_IR_DERIVES_FROM_CLAIM_H_
#define SRC_IR_DERIVES_FROM_CLAIM_H_

#include "src/ir/access_path.h"
#include "src/ir/edge.h"
#include "src/ir/proto/access_path.h"

namespace raksha::ir {

// A claim that a particular target_ AccessPath in a particular derives from the
// given source_. Multiple DerivesFrom can indicate the same target_, which
// shall indicate that that target derives only from the source_s given by
// the DerivesFromClaims. DerivesFrom claims exclude a particular target
// access path from the default edge-drawing behavior within a Particle, in
// which all inputs are connected to all outputs.
class DerivesFromClaim {
 public:
  explicit DerivesFromClaim(AccessPath target, AccessPath source)
    : target_(target), source_(source) {}

  const AccessPath &target() const { return target_; }

  const AccessPath &source() const { return source_; }

  // Internally, a DerivesFromClaim is basically just an explicitly-indicated
  // edge from one AccessPath to another. The GetAsEdge method returns the
  // Edge indicated by the DerivesFrom claim.
  Edge GetAsEdge() const {
    return Edge(source_, target_);
  }

 private:
  // The target_ AccessPath is the AccessPath that explicitly derives from
  // source_.
  AccessPath target_;
  AccessPath source_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_DERIVES_FROM_CLAIM_H_
