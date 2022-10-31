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

#include <limits>

#include "src/common/utils/map_iter.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"

namespace raksha::frontends::sql {

using ir::NamedAttributeMap;
using ir::NamedValueMap;
using ir::Operation;
using ir::Value;
using utils::MapIter;

const Operation &DecoderContext::MakeMergeOperation(
    std::vector<uint64_t> direct_input_ids,
    std::vector<uint64_t> control_input_ids) {
  auto get_value_for_id = [this](uint64_t id) { return GetValue(id); };
  return top_level_block_builder_.AddOperation<MergeOp>(
      ir_context_, MapIter<Value>(direct_input_ids, get_value_for_id),
      MapIter<Value>(control_input_ids, get_value_for_id));
}

const ir::Operation &DecoderContext::MakeTagTransformOperation(
    uint64_t transformed_value_id, absl::string_view rule_name,
    const common::containers::HashMap<std::string, uint64_t>
        &tag_preconditions) {
  auto precondition_value_pairs = MapIter<std::pair<std::string, Value>>(
      tag_preconditions,
      [this](const std::pair<std::string, uint64_t> &curr_pair) {
        return std::make_pair(curr_pair.first, GetValue(curr_pair.second));
      });
  return top_level_block_builder_.AddOperation<TagTransformOp>(
      ir_context_, rule_name, GetValue(transformed_value_id),
      std::move(precondition_value_pairs));
}

}  // namespace raksha::frontends::sql
