//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
#ifndef SRC_IR_DOT_DOT_GENERATOR_H_
#define SRC_IR_DOT_DOT_GENERATOR_H_

#include "src/ir/module.h"

namespace raksha::ir::dot {

class DotGenerator {
 public:
  std::string ModuleAsDot(const Module& module) const;
};

}  // namespace raksha::ir::dot

#endif  // SRC_IR_DOT_DOT_GENERATOR_H_
