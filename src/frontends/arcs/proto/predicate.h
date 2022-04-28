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

#ifndef SRC_FRONTENDS_ARCS_PROTO_PREDICATE_H_
#define SRC_FRONTENDS_ARCS_PROTO_PREDICATE_H_

#include <memory>

#include "src/frontends/arcs/access_path.h"
#include "src/frontends/arcs/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

// Decodes the predicate indicated by the given proto.
std::unique_ptr<Predicate> Decode(
    const ::arcs::InformationFlowLabelProto_Predicate &predicate_proto);

}  // namespace raksha::frontends::arcs::proto

#endif  // SRC_FRONTENDS_ARCS_PROTO_PREDICATE_H_
