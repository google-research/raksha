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
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/ir/attribute.h"
#include "src/ir/module.h"
#include "src/ir/ssa_names.h"

namespace raksha::ir {

// Note: I tried to make this only defined in terms of the type parameter T, but
// the compiler was unhappy with me writing std::function<std::string(const T&)>
// and I could not figure out how to placate it.
template<class T, class F>
static std::string PrintNamedMapInNameOrder(
    const absl::flat_hash_map<std::string, T> &map_to_print,
    F value_pretty_printer) {
  std::vector<absl::string_view> names;
  names.reserve(map_to_print.size());
  for (auto &key_value_pair : map_to_print) {
    names.push_back(key_value_pair.first);
  }
  std::sort(names.begin(), names.end());
  return absl::StrJoin(
      names, ", ",
      [&](std::string* out, const absl::string_view name) {
        absl::StrAppend(
            out, name, ": ", value_pretty_printer(map_to_print.at(name)));
      });
}

std::string Operation::ToString(SsaNames& ssa_names) const {
  constexpr absl::string_view kOperationFormat = "%%%d = %s [%s](%s)";
  SsaNames::ID this_ssa_name = ssa_names.GetOrCreateID(*this);

  // We want the attribute names to print in a stable order. This means that
  // we cannot just print from the attribute map directly. Gather the names
  // into a vector and sort that, then use the order in that vector to print
  // the attributes.
  std::string attributes_string =
      PrintNamedMapInNameOrder(attributes_,
          [](const Attribute &attr){ return attr->ToString(); });

  std::string inputs_string = PrintNamedMapInNameOrder(
      inputs_, [&](const Value &val){ return val.ToString(ssa_names); });

  return absl::StrFormat(kOperationFormat, this_ssa_name, op_->name(),
                         attributes_string, inputs_string);
}

}  // namespace raksha::ir
