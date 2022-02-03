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
#ifndef SRC_IR_OPERATOR_H_
#define SRC_IR_OPERATOR_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/ir/types/type.h"
#include "src/ir/block.h"
#include "src/ir/value.h"

namespace raksha::ir {

// Signature of an operator. An operator could be simple operators (e.g., `+`,
// `-`) or complex operators (e.g., particle, function) that is compose of other
// operations.
class Operator {
 public:
  Operator(absl::string_view name, std::unique_ptr<Block> implementation)
      : name_(name), implementation_(std::move(implementation)) {
    CHECK(implementation_ != nullptr);
  }

  const std::string& name() const { return name_; }
  const Block& implementation() const { return *implementation_.get(); }

 private:
  std::string name_;
  // If the implementation has no operations, then this is one of the basic
  // operators like `+`, `-`, etc., that cannot be split up any further.
  std::unique_ptr<Block> implementation_;
};

}  // namespace raksha::ir


#endif  // SRC_IR_OPERATOR_H_
