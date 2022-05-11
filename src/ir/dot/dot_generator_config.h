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
#ifndef SRC_IR_DOT_DOT_GENERATOR_CONFIG_H_
#define SRC_IR_DOT_DOT_GENERATOR_CONFIG_H_

#include <functional>
#include <string>
#include <vector>

#include "absl/container/btree_set.h"
#include "src/ir/module.h"

namespace raksha::ir::dot {

struct DotGeneratorConfig {
  std::function<std::string(const Operation&,
                            const absl::btree_set<std::string>&)>
      operation_labeler;

  // Default config.
  static DotGeneratorConfig GetDefault() {
    return {.operation_labeler =
                [](const Operation& op,
                   const absl::btree_set<std::string>& results) { return ""; }};
  }
};

}  // namespace raksha::ir::dot

#endif  // SRC_IR_DOT_DOT_GENERATOR_CONFIG_H_
