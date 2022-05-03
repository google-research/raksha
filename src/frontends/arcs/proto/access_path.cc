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
#include "src/frontends/arcs/proto/access_path.h"

#include "src/frontends/arcs/access_path.h"
#include "src/frontends/arcs/access_path_root.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/field_selector.h"
#include "src/ir/selector.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

//----------------
// Selector

ir::Selector Decode(::arcs::AccessPathProto_Selector selector) {
  if (selector.has_field()) {
    return ir::Selector(ir::FieldSelector(selector.field()));
  }
  LOG(FATAL) << "Found a Selector with an unimplemented specific type.";
}

// Make a Manifest proto Selector from this FieldSelector.
::arcs::AccessPathProto_Selector Encode(
    const ir::FieldSelector &field_selector) {
  ::arcs::AccessPathProto_Selector selector;
  selector.set_field(field_selector.field_name());
  return selector;
}

::arcs::AccessPathProto_Selector Encode(const ir::Selector &selector) {
  return std::visit(
      [](const auto &specific_selector) { return Encode(specific_selector); },
      selector.variant());
}

//----------------------
// AccessPathSelectors
ir::AccessPathSelectors DecodeSelectors(
    const ::arcs::AccessPathProto &access_path_proto) {
  const auto &selector_list = access_path_proto.selectors();
  ir::AccessPathSelectors access_path_selectors;
  for (auto iter = selector_list.rbegin(); iter != selector_list.rend();
       ++iter) {
    access_path_selectors = ir::AccessPathSelectors(
        Decode(*iter), std::move(access_path_selectors));
  }
  return access_path_selectors;
}

void EncodeSelectors(const ir::AccessPathSelectors &selectors,
                     ::arcs::AccessPathProto &access_path_proto) {
  for (const auto &selector : selectors) {
    *access_path_proto.add_selectors() = Encode(selector);
  }
}

//------------------
// AccessPathRoot

HandleConnectionSpecAccessPathRoot Decode(
    const ::arcs::AccessPathProto_HandleRoot &handle_root_proto) {
  std::string particle_spec_name = handle_root_proto.particle_spec();
  CHECK(!particle_spec_name.empty())
      << "Expected a HandleRoot message to have a non-empty particle_spec.";
  std::string handle_connection_spec_name =
      handle_root_proto.handle_connection();
  CHECK(!handle_connection_spec_name.empty())
      << "Expected a HandleRoot message to have a non-empty handle_connection.";
  return HandleConnectionSpecAccessPathRoot(particle_spec_name,
                                            handle_connection_spec_name);
}

::arcs::AccessPathProto_HandleRoot Encode(
    const HandleConnectionSpecAccessPathRoot &handle_root) {
  ::arcs::AccessPathProto_HandleRoot result;
  result.set_particle_spec(handle_root.particle_spec_name());
  result.set_handle_connection(handle_root.handle_connection_spec_name());
  return result;
}

//-----------------
// AccessPath

// We currently only handle the case in which an AccessPathProto is rooted at a
// (particle_spec, handle_connection).
AccessPath Decode(const ::arcs::AccessPathProto &access_path_proto) {
  CHECK(!access_path_proto.has_store_id())
      << "Currently, access paths involving stores are not implemented.";
  CHECK(access_path_proto.has_handle())
      << "Expected AccessPathProto to contain a handle member.";
  HandleConnectionSpecAccessPathRoot hcs_access_path_root =
      Decode(access_path_proto.handle());
  ir::AccessPathSelectors selectors = DecodeSelectors(access_path_proto);
  return AccessPath(AccessPathRoot(std::move(hcs_access_path_root)),
                    std::move(selectors));
}

}  // namespace raksha::frontends::arcs::proto
