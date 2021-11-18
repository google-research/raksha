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
#ifndef SRC_IR_SQL_QUERY_OPERATION_H_
#define SRC_IR_SQL_QUERY_OPERATION_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "src/ir/multi_operation.h"
#include "src/ir/operation.h"

namespace raksha::ir {

class SQLOperation : public Operation {
  SQLOperation(absl::string_view operation, std::vector<DataDecl> inputs,
               std::vector<DataDecl> outputs)
      : Operation(inputs, outputs), operation_(operation) {}

 private:
  std::string operation_;
};

class SQLQueryOperation : public Operation {
  SQLQueryOperation(std::vector<DataDecl> inputs, std::vector<DataDecl> outputs)
      : Operation(inputs, outputs), operations_(inputs, outputs) {}

 private:
  std::string query_;
  // The underlying AST of the sql query.
  MultiOperation operations_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SQL_QUERY_OPERATION_H_
