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
#ifndef SRC_FRONTENDS_ARCS_PROTO_ACCESS_PATH_H_
#define SRC_FRONTENDS_ARCS_PROTO_ACCESS_PATH_H_

#include "src/frontends/arcs/access_path.h"
#include "src/frontends/arcs/access_path_root.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

// Decodes the given arcs::AccessPathProto_Selector to a Selector.
ir::Selector Decode(::arcs::AccessPathProto_Selector selector);

::arcs::AccessPathProto_Selector Encode(const ir::Selector& selector);

// Returns an `AccessPathSelectors` instance from the given `AccessPathProto`.
ir::AccessPathSelectors DecodeSelectors(
    const ::arcs::AccessPathProto& access_path_proto);

// Encodes the given `selectors` into arcs::AccessPathProto.
void EncodeSelectors(const ir::AccessPathSelectors& selectors,
                     ::arcs::AccessPathProto& access_path_proto);

// Decodes the given `handle_root_proto` to `HandleConnectionAccessPathRoot`.
HandleConnectionSpecAccessPathRoot Decode(
    const ::arcs::AccessPathProto_HandleRoot& handle_root_proto);

// Encodes the given `handle_root` as arcs::AccessPathProto_HandleRoot.
::arcs::AccessPathProto_HandleRoot Encode(
    const HandleConnectionSpecAccessPathRoot& hande_root);

// Decodes the given `access_path_proto` as a AccessPath.
AccessPath Decode(const ::arcs::AccessPathProto& access_path_proto);

// Encodes the given `access_path` as an arcs::AccessPathProto.
::arcs::AccessPathProto Encode(const AccessPath& access_path);

}  // namespace raksha::frontends::arcs::proto

#endif  // SRC_FRONTENDS_ARCS_PROTO_ACCESS_PATH_H_
