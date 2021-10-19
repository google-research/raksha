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
//----------------------------------------------------------------------------

#ifndef SRC_IR_PROTO_PREDICATE_H_
#define SRC_IR_PROTO_PREDICATE_H_

#include <memory>

#include "src/ir/access_path.h"
#include "src/ir/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

// This class exists entirely to control access to the owning pointer for a
// Predicate. Predicates are owned by ParticleSpecs and should live as long
// as those ParticleSpecs. Therefore, individual Predicate unique_ptrs should
// not be handed out to the objects that wish to point to them; instead, they
// should be collected together in the decoder, which can then be kept alive
// as long as the ParticleSpec to which these predicates belong.
class PredicateDecoder {
 public:
  PredicateDecoder() : owned_predicates_() {}
    // Decode the predicate indicated by the given proto. Only a reference
    // will be returned; the owned pointer will be held and owned by this
    // object.
  const Predicate &Decode(
      const arcs::InformationFlowLabelProto_Predicate &predicate_proto);
 private:
  // The owned pointers are collected here, where they can be gathered by the
  // ParticleSpec later.
  std::vector<std::unique_ptr<Predicate>> owned_predicates_;

};

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_PREDICATE_H_
