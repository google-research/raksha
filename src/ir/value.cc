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
#include "src/ir/value.h"

#include <vector>

#include "absl/strings/str_format.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/ssa_names.h"

namespace raksha::ir {

std::string Value::ToString(SsaNames& ssa_names) const {
  return std::visit(
      raksha::utils::overloaded{
          [&ssa_names](const value::Any& any) {
            return any.ToString(ssa_names);
          },
          [&ssa_names](const value::StoredValue& stored_value) {
            return stored_value.ToString(ssa_names);
          },
          [this, &ssa_names](const value::BlockArgument& block_argument) {
            return absl::StrFormat("%s", ssa_names.GetOrCreateID(*this));
          },
          [this, &ssa_names](const value::OperationResult& operation_result) {
            return absl::StrFormat("%s", ssa_names.GetOrCreateID(*this));
          }},
      value_);
}
}  // namespace raksha::ir
