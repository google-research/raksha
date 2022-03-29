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
#ifndef SRC_FRONTENDS_SQL_OPS_TAG_TRANSFORM_OP_H_
#define SRC_FRONTENDS_SQL_OPS_TAG_TRANSFORM_OP_H_

#include "src/frontends/sql/ops/op_traits.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

class TagTransformOp : public SqlOp {
 public:
  static constexpr absl::string_view kRuleNameAttribute =
      "sql.tag_transform.rule_name";

  // Constructs a TagTransformOp.
  static std::unique_ptr<TagTransformOp> Create(
      const ir::Block* parent_block, const ir::IRContext& context,
      absl::string_view rule_name, ir::Value transformed_value,
      const std::vector<std::pair<std::string, ir::Value>>& preconditions);

  // Returns the value that is being tranformed by this op.
  ir::Value GetTransformedValue() const;

  // Returns the preconditions.
  absl::flat_hash_map<std::string, ir::Value> GetPreconditions() const;

  absl::string_view GetRuleName() const;

 private:
  // Inherit constructors.
  using SqlOp::SqlOp;
};

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_OPS_TAG_TRANSFORM_OP_H_
