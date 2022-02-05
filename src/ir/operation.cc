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
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/ir/attribute.h"
#include "src/ir/module.h"
#include "src/ir/ssa_names.h"

namespace raksha::ir {

std::string Operation::ToString(SSANames& ssa_names) const {
  constexpr absl::string_view kOperationFormat = "%%%d = %s [%s](%s)";
  SSANames::ID this_ssa_name = ssa_names.GetOrCreateID(*this);
  std::string attributes_string = absl::StrJoin(
      attributes_, ", ",
      [](std::string* out, const NamedAttributeMap::value_type& v) {
        const auto& [name, attribute] = v;
        absl::StrAppend(out, name, ": ", attribute->ToString());
      });

  std::string inputs_string = absl::StrJoin(
      inputs_, ", ",
      [&ssa_names](std::string* out, const NamedValueMap::value_type& v) {
        const auto& [name, value] = v;
        absl::StrAppend(out, name, ": ", value.ToString(ssa_names));
      });

  return absl::StrFormat(kOperationFormat, this_ssa_name, op_->name(),
                         attributes_string, inputs_string);
}

}  // namespace raksha::ir
