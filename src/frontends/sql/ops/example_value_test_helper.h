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
#ifndef SRC_FRONTENDS_SQL_OPS_EXAMPLE_VALUE_TEST_HELPER_H_
#define SRC_FRONTENDS_SQL_OPS_EXAMPLE_VALUE_TEST_HELPER_H_

#include "absl/strings/string_view.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql::testing {

// Class that will help generate example values.
class ExampleValueTestHelper {
 public:
  ExampleValueTestHelper()
      : operator_("example"), operation_(nullptr, operator_, {}, {}) {}

  ir::Value GetOperationResult(absl::string_view name) const {
    return ir::Value(ir::value::OperationResult(operation_, name));
  }

  ir::Value GetBlockArgument(absl::string_view name) const {
    return ir::Value(ir::value::BlockArgument(block_, name));
  }

  ir::Value GetAny() const { return ir::Value(ir::value::Any()); }

  const ir::Block& GetBlock() const { return block_; }

 private:
  ir::Operator operator_;
  ir::Operation operation_;
  ir::Block block_;
};

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_OPS_EXAMPLE_VALUE_TEST_HELPER_H_
