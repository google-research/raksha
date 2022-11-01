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

#include "src/frontends/sql/ops/literal_op.h"

#include <memory>

#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

std::unique_ptr<LiteralOp> LiteralOp::Create(const ir::Block* parent_block,
                                             const ir::IRContext& context,
                                             std::string_view literal) {
  return std::make_unique<LiteralOp>(
      parent_block, *CHECK_NOTNULL(SqlOp::GetOperator<LiteralOp>(context)),
      ir::NamedAttributeMap(
          {{std::string(kLiteralStringAttrName),
            ir::Attribute::Create<ir::StringAttribute>(literal)}}),
      ir::ValueList({}),
      /*impl_module=*/nullptr);
}

std::string_view LiteralOp::GetLiteralString() const {
  auto find_result = attributes().find(kLiteralStringAttrName);
  CHECK(find_result != attributes().end());
  auto literal = find_result->second;
  auto string_literal = CHECK_NOTNULL(literal.GetIf<ir::StringAttribute>());
  return string_literal->value();
}

}  // namespace raksha::frontends::sql
