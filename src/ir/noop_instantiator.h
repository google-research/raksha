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

#ifndef SRC_IR_NOOP_INSTANTIATOR_H_
#define SRC_IR_NOOP_INSTANTIATOR_H_

#include "src/ir/access_path_root.h"
#include "src/ir/instantiator.h"

namespace raksha::ir {

// This trivial implementation of Instantiator does no instantiation. It just
// checks that the AccessPathRoot is already instantiated. There is one
// single canonical instance of it.
class NoopInstantiator : public Instantiator {
 public:
  static NoopInstantiator Get() {
    static NoopInstantiator instance_;
    return instance_;
  }

  const AccessPathRoot &GetInstantiatedRoot(
      const AccessPathRoot &original) const {
    CHECK(original.IsInstantiated())
      << "Attempt to instantiate a non-instantiated root with the "
      << "NoopInstantiator!";
    return original;
  }
 private:
  NoopInstantiator() {}
};

}  // namespace raksha::ir

#endif  // SRC_IR_NOOP_INSTANTIATOR_H_
