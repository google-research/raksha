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

#ifndef SRC_IR_INSTANTIATOR_H_
#define SRC_IR_INSTANTIATOR_H_

#include "absl/container/flat_hash_map.h"

namespace raksha::ir {

// Forward declaration needed to break inclusion cycle between AccessPathRoot
// and Instantiator.
class AccessPathRoot;

// Classes that implement this interface can provide an instantiated root
// that should be used in place of an uninstantiated root.
class Instantiator {
 public:
  virtual ~Instantiator() {}

  virtual const AccessPathRoot &GetInstantiatedRoot(
      const AccessPathRoot &original_root) const = 0;
};

}  // namespace raksha::ir

#endif  // SRC_IR_INSTANTIATOR_H_
