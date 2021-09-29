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

#ifndef SRC_IR_MAP_INSTANTIATOR_H_
#define SRC_IR_MAP_INSTANTIATOR_H_

#include "absl/container/flat_hash_map.h"
#include "src/ir/instantiator.h"

namespace raksha::ir {

// An instantiator that just directly holds an instantiation map. In the
// future, we probably want to prefer to use an Instantiator that corresponds
// to some language feature (like a Particle in the Arcs manifest) but for
// now, this allows us to perform instantiation without having to directly
// tackle refactoring our traversal of arcs protos (issue #107).
class MapInstantiator : public Instantiator {
 public:
  MapInstantiator(absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
      instantiation_map) : instantiation_map_(std::move(instantiation_map)) {}

  const AccessPathRoot &GetInstantiatedRoot(
      const AccessPathRoot &original_root) const override {
    auto find_res = instantiation_map_.find(original_root);
    CHECK(find_res != instantiation_map_.end())
      << "Could not find match to instantiate original root"
      << " in GetInstantiatedRoot.";
    return find_res->second;
  }

 private:
  const absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
      instantiation_map_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_MAP_INSTANTIATOR_H_
