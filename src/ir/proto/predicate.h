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
#include "src/ir/predicates/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

// Decode the predicate which applies to the given AccessPath. While all
// terms in the Arcs manifest implicitly apply to a single access_path, we
// would like to ensure that if we encounter systems that need to check the
// presence of tags on separate access paths in the same expression, we have
// the flexibility of doing so; therefore, our TagPresence class holds the
// AccessPath about which it speaks and we must provide the AccessPath to
// attach to it here.
std::unique_ptr<raksha::ir::predicates::Predicate> Decode(
    const arcs::InformationFlowLabelProto_Predicate &predicate_proto);

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_PREDICATE_H_
