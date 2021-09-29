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

#include "src/ir/access_path_root.h"

#include "src/ir/instantiator.h"
#include "src/ir/noop_instantiator.h"

namespace raksha::ir {

// Replace the root with the instantiated root provided by instantiator, then
// call ToString on that root. Pass in the NoopInstantiator rather than the
// given instantiator because we expect the returned root to already be
// instantiated.
std::string HandleConnectionSpecAccessPathRoot::ToString(
    const Instantiator &instantiator) const {
  return instantiator.GetInstantiatedRoot(AccessPathRoot(*this))
    .ToString(NoopInstantiator::Get());
}

}  // namespace raksha::ir
