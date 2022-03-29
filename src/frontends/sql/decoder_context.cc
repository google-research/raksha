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

const Operation &DecoderContext::MakeMergeOperation(
    std::vector<Value> direct_inputs, std::vector<Value> control_inputs) {
  return top_level_block_builder_.AddOperation<MergeOp>(
      ir_context_, std::move(direct_inputs), std::move(control_inputs));
}

const ir::Operation &DecoderContext::MakeTagTransformOperation(
    ir::Value transformed_value, absl::string_view rule_name,
    const absl::flat_hash_map<std::string, uint64_t> &precondition_indices) {
  std::vector<std::pair<std::string, ir::Value>> preconditions;
  for (const auto &[name, id] : precondition_indices) {
    preconditions.push_back({name, GetValue(id)});
  }
  return top_level_block_builder_.AddOperation<TagTransformOp>(
      ir_context_, rule_name, transformed_value, preconditions);
}

}  // namespace raksha::frontends::sql
