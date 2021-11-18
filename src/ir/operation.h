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
#ifndef SRC_IR_OPERATION_H_
#define SRC_IR_OPERATION_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "absl/container/flat_hash_map.h"
#include "src/ir/access_path_v2.h"
#include "src/ir/data_decl.h"

namespace raksha::ir {

// Description of an operation in the IR.
class Operation {
 public:
  Operation(std::vector<DataDecl> inputs, std::vector<DataDecl> outputs)
      : inputs_(std::move(inputs)), outputs_(std::move(outputs)) {}

 protected:
  // Connects an access path to the corresponding input.
  void AddInput(absl::string_view name, v2::AccessPath access_path);

  // Connects an output to the corresponding access path.
  void AddOutput(absl::string_view name, v2::AccessPath access_path);

 private:
  // Kind kind_; TODO: we should add a kind.
  // We should also add a visitor for operations.
  std::vector<DataDecl> inputs_;
  std::vector<DataDecl> outputs_;
  absl::flat_hash_map<std::string, std::vector<v2::AccessPath>> inputs_edges_;
  absl::flat_hash_map<std::string, std::vector<v2::AccessPath>> output_edges_;
};

}  // namespace raksha::ir


#endif  // SRC_IR_OPERATION_H_
