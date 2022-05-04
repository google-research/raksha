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
#include "src/frontends/sql/ops/merge_op.h"

#include <limits>
#include <memory>
#include <type_traits>

#include "absl/algorithm/container.h"
#include "src/common/utils/ranges.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

ir::ValueList MakeCombinedInputs(ir::ValueList direct_inputs,
                                 ir::ValueList control_inputs) {
  ir::ValueList inputs;
  absl::c_move(direct_inputs, std::back_inserter(inputs));
  absl::c_move(control_inputs, std::back_inserter(inputs));
  return inputs;
}

static int64_t GetControlInputStartIndexFromDirectInputListSize(
    size_t direct_inputs_size) {
  int64_t control_start_index = static_cast<int64_t>(direct_inputs_size);
  return control_start_index;
}

MergeOp::MergeOp(const ir::Block* parent_block, const ir::IRContext& context,
                 ir::ValueList direct_inputs, ir::ValueList control_inputs)
    : SqlOp(parent_block, *CHECK_NOTNULL(SqlOp::GetOperator<MergeOp>(context)),
            ir::NamedAttributeMap(
                {{std::string(kControlInputStartIndex),
                  ir::Attribute::Create<ir::Int64Attribute>(
                      GetControlInputStartIndexFromDirectInputListSize(
                          direct_inputs.size()))}}),
            // Note: if we move `direct_inputs` here, we destroy the contents of
            // the vector which are needed for initializing the attribute list.
            // Unfortunately, there is no guarantee on the order in which the
            // arguments to this parent ctor will be evaluated. We should see if
            // we can get rid of this copy somehow.
            MakeCombinedInputs(direct_inputs, std::move(control_inputs))) {
  // Do some sanity checks.
  CHECK(!inputs().empty())
      << "Both direct inputs and control inputs are empty.";
  CHECK(inputs().size() < std::numeric_limits<int64_t>::max())
      << "Too many direct_inputs.";  // We will not test this.
}

ir::ValueRange MergeOp::GetDirectInputs() const {
  return utils::make_range(inputs().begin(),
                           inputs().begin() + GetControlInputStartIndex());
}

ir::ValueRange MergeOp::GetControlInputs() const {
  return utils::make_range(inputs().begin() + GetControlInputStartIndex(),
                           inputs().end());
}

size_t MergeOp::GetControlInputStartIndex() const {
  const auto& attributes_map = attributes();
  auto find_result = attributes_map.find(kControlInputStartIndex);
  CHECK(find_result != attributes_map.end());
  auto int_attribute =
      CHECK_NOTNULL(find_result->second.GetIf<ir::Int64Attribute>());

  CHECK(int_attribute->value() >= 0);
  return static_cast<size_t>(int_attribute->value());
}

}  // namespace raksha::frontends::sql
