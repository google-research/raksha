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
#ifndef SRC_IR_STORAGE_H_
#define SRC_IR_STORAGE_H_

#include "src/ir/types/type.h"

namespace raksha::ir {

// A class that represents a storage.
class Storage {
 public:
  Storage(types::Type type) : type_(std::move(type)) {}

  // Disable copy (and move) semantics.
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

 private:
  types::Type type_;
  // TODO: We will add additional attributes like ttl, medium, etc.
};

}  // namespace raksha::ir

#endif  // SRC_IR_STORAGE_H_
