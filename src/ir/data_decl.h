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
#ifndef SRC_IR_DATA_DECL_H_
#define SRC_IR_DATA_DECL_H_

#include <string>

#include "src/ir/types/type.h"

namespace raksha::ir {

// Declaration of a data.
class DataDecl {
  DataDecl(std::string name, const types::Type* type)
      : name_(std::move(name)), type_(type) {}

 private:
  std::string name_;
  const types::Type* type_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_DATA_DECL_H_
