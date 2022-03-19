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
#ifndef SRC_FRONTENDS_SQL_OPS_LITERAL_OP_H_
#define SRC_FRONTENDS_SQL_OPS_LITERAL_OP_H_

#include <memory>

#include "src/frontends/sql/ops/sql_op.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"

namespace raksha::frontends::sql {

class LiteralOp : public SqlOp {
 public:
  static constexpr absl::string_view kLiteralStringAttrName = "literal_string";

  // Constructs a LiteralOp.
  static std::unique_ptr<LiteralOp> Create(const ir::Block* parent_block,
                                           const ir::IRContext& context,
                                           absl::string_view literal);

  // Inherit constructors.
  using SqlOp::SqlOp;

  // Returns the literal string associated with the operation.
  absl::string_view GetLiteralString() const;
};

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_OPS_LITERAL_OP_H_
