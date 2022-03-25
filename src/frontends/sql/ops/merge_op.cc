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

std::unique_ptr<MergeOp> MergeOp::Create(const ir::Block* parent_block,
                                         const ir::IRContext& context,
                                         ir::ValueList direct_inputs,
                                         ir::ValueList control_inputs) {
  // Do some sanity checks.
  CHECK(!direct_inputs.empty() || !control_inputs.empty())
      << "Both direct inputs and control inputs are empty.";
  CHECK(direct_inputs.size() < std::numeric_limits<int64_t>::max())
      << "Too many direct_inputs.";  // We will not test this.

  // Now remember the index for the start of control inputs.
  int64_t control_start_index = static_cast<int64_t>(direct_inputs.size());

  // Move direct_inputs and control_inputs to inputs.
  ir::ValueList inputs;
  absl::c_move(direct_inputs, std::back_inserter(inputs));
  absl::c_move(control_inputs, std::back_inserter(inputs));

  // Construct and return the operation
  return std::make_unique<MergeOp>(
      parent_block, *CHECK_NOTNULL(SqlOp::GetOperator<MergeOp>(context)),
      ir::NamedAttributeMap(
          {{std::string(kControlInputStartIndex),
            ir::Attribute::Create<ir::Int64Attribute>(control_start_index)}}),
      std::move(inputs));
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
  auto find_result = attributes().find(kControlInputStartIndex);
  CHECK(find_result != attributes().end());
  auto int_attribute =
      CHECK_NOTNULL(find_result->second.GetIf<ir::Int64Attribute>());

  CHECK(int_attribute->value() >= 0);
  return static_cast<size_t>(int_attribute->value());
}

}  // namespace raksha::frontends::sql
