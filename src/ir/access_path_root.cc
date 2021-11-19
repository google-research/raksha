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

#include "src/ir/datalog_print_context.h"

// The classes in this file describe the root of an AccessPath. At the moment,
// we have only two types of roots: a HandleConnectionSpecAccessPathRoot and
// a HandleConnectionAccessPathRoot, describing a HandleConnectionSpec in a
// ParticleSpec and a Handle in a Particle, respectively.
namespace raksha::ir {

std::string AccessPathRoot::ToDatalog(const DatalogPrintContext &ctxt) const {
  const auto *instantiation_map = ctxt.instantiation_map();
  if (instantiation_map == nullptr) return this->ToString();
  auto find_res = instantiation_map->find(*this);
  return (find_res != instantiation_map->end()) ? find_res->second.ToString()
                                                : this->ToString();
}

}  // namespace raksha::ir
