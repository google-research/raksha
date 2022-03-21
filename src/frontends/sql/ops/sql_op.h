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
#ifndef SRC_FRONTENDS_SQL_OPS_SQL_OP_H_
#define SRC_FRONTENDS_SQL_OPS_SQL_OP_H_

#include <optional>

#include "src/frontends/sql/ops/op_traits.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::frontends::sql {

// Base class for all SQL-related operators.
class SqlOp : public ir::Operation {
 public:
  // Returns the operator associated with this type `T`.
  template <typename T>
  static const ir::Operator& GetOperator(const ir::IRContext& context) {
    absl::string_view operator_name = OpTraits<T>::kName;
    CHECK(context.IsRegisteredOperator(operator_name))
        << "SqlOp '" << operator_name << "' is not registered.";
    return context.GetOperator(operator_name);
  }

  // Registers the operator for this type.
  template <typename T>
  static void RegisterOperator(ir::IRContext& context) {
    context.RegisterOperator(
        std::make_unique<ir::Operator>(OpTraits<T>::kName));
  }

  // Inherit constructors.
  using ir::Operation::Operation;

  virtual ~SqlOp() {}
};

}  // namespace raksha::frontends::sql
#endif  // SRC_FRONTENDS_SQL_OPS_SQL_OP_H_
