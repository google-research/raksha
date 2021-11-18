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
#ifndef SRC_IR_ACCESS_PATH_V2_H_
#define SRC_IR_ACCESS_PATH_V2_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "absl/container/flat_hash_map.h"
#include "src/ir/access_path_v2.h"
#include "src/ir/types/type.h"

namespace raksha::ir {


class Operation;
class Storage;

// Namespace for the time being.
namespace v2 {

// TODO: Rough sketch. We will need to flesh this out more.
// Some thoughts:
//  - We may not need to use ComputeNode/StorageNode, if we have global name/id.
//  - Selectors is an std::vector. A linked list might be better.
class AccessPath {
 public:
  class Base {
   private:
    std::variant<const Operation*, const Storage*> node_;
    std::string name_;
  };

  class Selectors {
   private:
    // TODO: a richer selector type like index, field, dereference, etc.?
    std::vector<std::string> selectors_;
  };

 private:
  const types::Type* type_;
  Base base_;
  Selectors selectors_;
};

}  // namespace v2

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_V2_H_
