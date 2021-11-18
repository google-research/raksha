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
#ifndef SRC_IR_TAG_CLAIM_OPERATION_H_
#define SRC_IR_TAG_CLAIM_OPERATION_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "src/ir/access_path_v2.h"
#include "src/ir/data_decl.h"
#include "src/ir/operation.h"

namespace raksha::ir {

// Description of an operation in the IR.
class TagClaimOperation : public Operation {
 public:

  TagClaimOperation(const std::vector<DataDecl>& data)
      // Inputs and outputs are the same.
      : Operation(data, data) {}

  // A visitor for datalog would dump the corresponding datalog facts
  // for the claims.

  // Adds a claim name.selectors is predicate.
  // void AddPredicate(absl::string_view name,
  //                   const v2::AccessPath::Selectors& selectors,
  //                   std::unique_ptr<Predicate> predicate);
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_OPERATION_H_
