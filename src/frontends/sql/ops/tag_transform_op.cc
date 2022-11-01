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
#include "src/frontends/sql/ops/tag_transform_op.h"

#include <iterator>
#include <limits>

#include "src/frontends/sql/ops/op_traits.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

std::unique_ptr<TagTransformOp> TagTransformOp::Create(
    const ir::Block* parent_block, const ir::IRContext& context,
    std::string_view rule_name, ir::Value transformed_value,
    const std::vector<std::pair<std::string, ir::Value>>& preconditions) {
  ir::ValueList inputs;
  ir::NamedAttributeMap attributes;
  inputs.push_back(transformed_value);
  int64_t index = 1;
  attributes.insert({std::string(kRuleNameAttribute),
                     ir::Attribute::Create<ir::StringAttribute>(rule_name)});
  CHECK(preconditions.size() < std::numeric_limits<int64_t>::max());
  for (const auto& [name, value] : preconditions) {
    inputs.push_back(value);
    auto insert_result = attributes.insert(
        {name, ir::Attribute::Create<ir::Int64Attribute>(index++)});
    CHECK(insert_result.second)
        << "Duplicate precondition name '" << name << "'.";
  }

  return std::make_unique<TagTransformOp>(
      parent_block,
      *CHECK_NOTNULL(context.GetOperator(OpTraits<TagTransformOp>::kName)),
      attributes, inputs);
}

ir::Value TagTransformOp::GetTransformedValue() const {
  CHECK(inputs().size() > 0);
  return inputs().front();
}

common::containers::HashMap<std::string, ir::Value>
TagTransformOp::GetPreconditions() const {
  common::containers::HashMap<std::string, ir::Value> result;
  const ir::ValueList& input_values = inputs();
  for (const auto& [name, attribute] : attributes()) {
    if (name == kRuleNameAttribute) continue;
    auto int_attribute =
        CHECK_NOTNULL(attribute.GetIf<ir::Int64Attribute>());
    auto input_index = int_attribute->value();
    // If input_values.size() > std::numeric_limits<int64_t>::max(), the
    // static_cast will return a negative value and this condition will fail,
    // which is desired.
    CHECK(input_index >= 1 &&
          input_index < static_cast<int64_t>(input_values.size()))
        << "Invalid index in TagTransformOp";
    result.insert({name, input_values[input_index]});
  }
  return result;
}

std::string_view TagTransformOp::GetRuleName() const {
  const ir::NamedAttributeMap& attribute_map = attributes();
  auto find_result = attribute_map.find(kRuleNameAttribute);
  CHECK(find_result != attribute_map.end());
  auto rule_name_attribute =
      CHECK_NOTNULL(find_result->second.GetIf<ir::StringAttribute>());
  return rule_name_attribute->value();
}

}  // namespace raksha::frontends::sql
