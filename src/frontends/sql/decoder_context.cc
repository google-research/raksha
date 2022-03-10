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

#include "src/frontends/sql/decoder_context.h"

#include "src/utils/map_iter.h"

namespace raksha::frontends::sql {

using ir::NamedValueMap;
using ir::Operation;
using ir::Value;

static NamedValueMap MakeNamedValueMapFromVector(absl::string_view prefix,
                                                 std::vector<Value> vec,
                                                 NamedValueMap map) {
  uint64_t i = 0;
  uint64_t combined_size = vec.size() + map.size();
  map.merge(
      utils::MapIterVal<NamedValueMap>(std::move(vec), [&](ir::Value value) {
        uint64_t current_idx = i;
        ++i;
        return std::make_pair(absl::StrCat(prefix, current_idx), value);
      }));
  CHECK(map.size() == combined_size)
      << "Saw collisions in combining elements from vectors";
  return map;
}

const Operation &DecoderContext::MakeMergeOperation(
    std::vector<Value> direct_inputs, std::vector<Value> control_inputs) {
  // Add the direct and control inputs into the merge with predictable
  // prefixes.
  return top_level_block_builder_.AddOperation(
      merge_operator_,
      /*attributes=*/{},
      MakeNamedValueMapFromVector(
          "control_input_", std::move(control_inputs),
          MakeNamedValueMapFromVector("direct_input_", std::move(direct_inputs),
                                      {})));
}

}  // namespace raksha::frontends::sql
