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

#ifndef SRC_FRONTENDS_SQL_TESTING_LITERAL_OPERATION_VIEW_H_
#define SRC_FRONTENDS_SQL_TESTING_LITERAL_OPERATION_VIEW_H_

#include "src/common/logging/logging.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/testing/operation_view_utils.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::frontends::sql::testing {

// Construct a view onto an `Operation` that happens to be a `MergeOperation`
// that provides a richer interface to the `MergeOperation`.
class LiteralOperationView {
 public:
  LiteralOperationView(const ir::Operation &literal_operation)
      : literal_operation_(&literal_operation) {
    CHECK(literal_operation.op().name() == DecoderContext::kSqlLiteralOpName);
    CHECK(literal_operation.attributes().size() == 1);
    CHECK(literal_operation.inputs().size() == 0);
  }

  absl::string_view GetLiteralStr() const {
    const ir::NamedAttributeMap &attrs = literal_operation_->attributes();
    auto find_result = attrs.find(DecoderContext::kLiteralStrAttrName);
    CHECK(find_result != attrs.end());
    const auto &string_attribute =
        CHECK_NOTNULL(find_result->second.GetIf<ir::StringAttribute>());
    return string_attribute->value();
  }

 private:
  const ir::Operation *literal_operation_;
};

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_LITERAL_OPERATION_VIEW_H_
