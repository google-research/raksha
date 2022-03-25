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

#ifndef SRC_FRONTENDS_SQL_TESTING_MERGE_OPERATION_VIEW_H_
#define SRC_FRONTENDS_SQL_TESTING_MERGE_OPERATION_VIEW_H_

#include "src/common/logging/logging.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/testing/operation_view_utils.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::frontends::sql::testing {

// Construct a view onto an `Operation` that happens to be a `MergeOperation`
// that provides a richer interface to the `MergeOperation`.
class MergeOperationView {
 public:
  MergeOperationView(const ir::Operation &merge_operation)
      : merge_operation_(&merge_operation) {
    CHECK(merge_operation.op().name() == DecoderContext::kSqlMergeOpName);
    CHECK(merge_operation.attributes().count(
              DecoderContext::kMergeOpControlStartIndex) > 0);
  }

  std::vector<ir::Value> GetDirectInputs() const {
    auto attr_result = merge_operation_->attributes().find(
        DecoderContext::kMergeOpControlStartIndex);
    CHECK(attr_result != merge_operation_->attributes().end());
    auto control_start_index =
        CHECK_NOTNULL(attr_result->second.GetIf<ir::Int64Attribute>());
    return std::vector<ir::Value>(
        merge_operation_->inputs().begin(),
        merge_operation_->inputs().begin() + control_start_index->value());
  }

  std::vector<ir::Value> GetControlInputs() const {
    auto attr_result = merge_operation_->attributes().find(
        DecoderContext::kMergeOpControlStartIndex);
    CHECK(attr_result != merge_operation_->attributes().end());
    auto control_start_index =
        CHECK_NOTNULL(attr_result->second.GetIf<ir::Int64Attribute>());
    return std::vector<ir::Value>(
        merge_operation_->inputs().begin() + control_start_index->value(),
        merge_operation_->inputs().end());
  }

 private:
  const ir::Operation *merge_operation_;
};

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_MERGE_OPERATION_VIEW_H_
