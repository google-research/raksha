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

#ifndef SRC_FRONTENDS_SQL_TESTING_TAG_TRANSFORM_OPERATION_VIEW_H_
#define SRC_FRONTENDS_SQL_TESTING_TAG_TRANSFORM_OPERATION_VIEW_H_

#include "src/common/logging/logging.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/testing/operation_view_utils.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::frontends::sql::testing {

// Construct a view onto an `Operation` that happens to be a
// `TagTransformOperation` that provides a richer interface to the
// `TagTransformOperation`.
class TagTransformOperationView {
 public:
  TagTransformOperationView(const ir::Operation &tag_transform_operation)
      : tag_transform_operation_(&tag_transform_operation) {
    CHECK(tag_transform_operation.op().name() ==
          DecoderContext::kTagTransformOpName);
    CHECK(tag_transform_operation.attributes().size() == 1);
  }

  ir::Value GetTransformedValue() const {
    const ir::NamedValueMap &inputs = tag_transform_operation_->inputs();
    auto find_result =
        inputs.find(DecoderContext::kTagTransformTransformedValueInputName);
    CHECK(find_result != inputs.end());
    return find_result->second;
  }

  std::vector<ir::Value> GetPreconditions() const {
    return GetVecWithPrefix(
        tag_transform_operation_->inputs(),
        DecoderContext::kTagTransformPreconditionInputPrefix);
  }

  std::string GetRuleName() const {
    const ir::NamedAttributeMap &attributes =
        tag_transform_operation_->attributes();
    auto find_result =
        attributes.find(DecoderContext::kTagTransformRuleAttributeName);
    CHECK(find_result != attributes.end());
    return find_result->second.ToString();
  }

 private:
  const ir::Operation *tag_transform_operation_;
};

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_TAG_TRANSFORM_OPERATION_VIEW_H_
