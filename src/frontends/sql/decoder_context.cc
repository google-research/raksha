#include "src/frontends/sql/decoder_context.h"

#include <limits>

#include "src/common/utils/map_iter.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"

namespace raksha::frontends::sql {

using ir::NamedAttributeMap;
using ir::NamedValueMap;
using ir::Operation;
using ir::Value;

const Operation &DecoderContext::MakeMergeOperation(
    std::vector<Value> direct_inputs, std::vector<Value> control_inputs) {
  size_t control_start_index = direct_inputs.size();
  CHECK(control_start_index < std::numeric_limits<int64_t>::max());
  auto attributes =
      ir::NamedAttributeMap({{std::string(kMergeOpControlStartIndex),
                              ir::Attribute::Create<ir::Int64Attribute>(
                                  static_cast<int64_t>(control_start_index))}});
  // Combine the direct and control inputs.
  absl::c_move(control_inputs, std::back_inserter(direct_inputs));
  return top_level_block_builder_.AddOperation(
      merge_operator_, std::move(attributes), std::move(direct_inputs));
}

const ir::Operation &DecoderContext::MakeTagTransformOperation(
    ir::Value transformed_value, absl::string_view rule_name,
    const absl::flat_hash_map<std::string, uint64_t> &preconditions) {
  // We need to reserve one space for the transformed value plus one for each
  // precondition.
  ir::ValueList inputs;
  // Not adding +1 to avoid potential overflow.
  inputs.reserve(preconditions.size());

  // Insert the value to be transformed.
  inputs.push_back(transformed_value);

  // Each precondition is given the same input name prefix plus an incrementing
  // number in the same order as in the original ID list.
  ir::NamedAttributeMap attributes;

  attributes.insert({std::string(kTagTransformRuleAttributeName),
                     ir::Attribute::Create<ir::StringAttribute>(rule_name)});
  int64_t index = 1;
  for (const auto &[name, id] : preconditions) {
    CHECK(index != std::numeric_limits<int64_t>::max())
        << "`int64_t` might overflow. Why would you create so many attributes?";
    inputs.push_back(GetValue(id));
    auto insert_result = attributes.insert(
        {name, ir::Attribute::Create<ir::Int64Attribute>(index++)});
    CHECK(insert_result.second)
        << "Found duplicate entry for " << insert_result.first->first;
  }

  return top_level_block_builder_.AddOperation(tag_transform_operator_,
                                               attributes, inputs, {});
}

}  // namespace raksha::frontends::sql
