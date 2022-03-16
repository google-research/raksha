#include "src/frontends/sql/decoder_context.h"

#include "src/common/utils/map_iter.h"

namespace raksha::frontends::sql {

using ir::NamedAttributeMap;
using ir::NamedValueMap;
using ir::Operation;
using ir::Value;

static NamedValueMap MakeNamedValueMapFromVector(absl::string_view prefix,
                                                 std::vector<Value> vec,
                                                 NamedValueMap map) {
  uint64_t i = 0;
  for (auto iter = std::make_move_iterator(vec.begin());
       iter != std::make_move_iterator(vec.end()); ++iter) {
    auto insert_result = map.insert({absl::StrCat(prefix, i), *iter});
    CHECK(insert_result.second)
        << "Collision on key " << insert_result.first->first;
    ++i;
  }
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

const ir::Operation &DecoderContext::MakeTagTransformOperation(
    ir::Value transformed_value, absl::string_view rule_name,
    std::vector<uint64_t> preconditions) {
  // We need to reserve one space for the transformed value plus one for each
  // precondition.
  NamedValueMap values_map;
  values_map.reserve(preconditions.size() + 1);

  // Insert the value to be transformed.
  values_map.insert(
      {std::string(kTagTransformTransformedValueInputName), transformed_value});

  // Look up each precondition ID to get the corresponding Value.
  std::vector<ir::Value> precondition_values_vec = utils::MapIter<ir::Value>(
      preconditions, [&](uint64_t id) { return GetValue(id); });

  // Each precondition is given the same input name prefix plus an incrementing
  // number in the same order as in the original ID list.
  for (uint64_t i = 0; i < precondition_values_vec.size(); ++i) {
    auto insert_result = values_map.insert(
        {absl::StrCat(kTagTransformPreconditionInputPrefix, i),
         precondition_values_vec.at(i)});
    CHECK(insert_result.second)
        << "Found duplicate entry for " << insert_result.first->first;
  }

  return top_level_block_builder_.AddOperation(
      tag_transform_operator_,
      NamedAttributeMap{{std::string(kTagTransformRuleAttributeName),
                         ir::StringAttribute::Create(rule_name)}},
      std::move(values_map));
}

}  // namespace raksha::frontends::sql
