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
#ifndef SRC_IR_MULTI_OPERATION_H_
#define SRC_IR_MULTI_OPERATION_H_

#include <memory>
#include <vector>

#include "src/ir/data_decl.h"
#include "src/ir/operation.h"

namespace raksha::ir {

// An operation that is composed of more than one operation. The
// operations themselves will form a graph of dataflow computation.
class MultiOperation : public Operation {
 public:
  MultiOperation(std::vector<DataDecl> inputs, std::vector<DataDecl> outputs)
      : Operation(inputs, outputs) {}

  // void AddOperation(std::unique_ptr<Operation> operation);
  // API to Connect Edges...
 private:
  std::vector<std::unique_ptr<Operation>> operations_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_MULTI_OPERATION_H_
