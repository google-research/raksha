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
#ifndef SRC_FRONTENDS_SQL_OPS_MERGE_OP_H_
#define SRC_FRONTENDS_SQL_OPS_MERGE_OP_H_

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

class MergeOp : public SqlOp {
 public:
  static constexpr absl::string_view kControlInputStartIndex =
      "control_input_start_index";

  // Constructs a MergeOp.
  MergeOp(const ir::Block* parent_block, const ir::IRContext& context,
          ir::ValueList direct_inputs, ir::ValueList control_inputs);

  static std::unique_ptr<MergeOp> Create(const ir::Block* parent_block,
                                         const ir::IRContext& context,
                                         ir::ValueList direct_inputs,
                                         ir::ValueList control_inputs) {
    return std::make_unique<MergeOp>(parent_block, context,
                                     std::move(direct_inputs),
                                     std::move(control_inputs));
  }

  // Returns an iterator range for the direct inputs.
  ir::ValueRange GetDirectInputs() const;

  // Returns an iterator range for the control inputs.
  ir::ValueRange GetControlInputs() const;

  // Returns the index where the control inputs start.
  size_t GetControlInputStartIndex() const;
};

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_OPS_MERGE_OP_H_
