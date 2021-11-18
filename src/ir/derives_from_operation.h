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
#ifndef SRC_IR_DERIVES_FROM_OPERATION_H_
#define SRC_IR_DERIVES_FROM_OPERATION_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "src/ir/access_path_v2.h"
#include "src/ir/data_decl.h"
#include "src/ir/operation.h"

namespace raksha::ir {

class DerivesFromOperation : public Operation {
  // The inputs and outputs are the same.
  explicit DerivesFromOperation(const std::vector<DataDecl>& data)
      : Operation(data, data) {}

  DerivesFromOperation(const std::vector<DataDecl>& data, bool allow_all_edges)
      : Operation(data, data) {
    // Add all possible allowed edges between inputs and outputs.
  }


  // Add the given to the allowed internal edges.
  void AddAllowedEdge(absl::string_view input,
                      const v2::AccessPath::Selectors& input_selectors,
                      absl::string_view output,
                      const v2::AccessPath::Selectors& output_selectors);

 private:
  // TODO: Keep tracking of eliminated edges might be more safe and efficient.
  std::vector<std::pair<v2::AccessPath, v2::AccessPath>> allowed_edges_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_DERIVES_FROM_OPERATION_H_
