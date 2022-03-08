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

#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "src/common/logging/logging.h"
#include "src/frontends/sql/decoder_context.h"
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
    CHECK(merge_operation.attributes().empty());
  }

  std::vector<ir::Value> GetDirectInputs() const {
    return GetVecWithPrefix(merge_operation_->inputs(),
                            DecoderContext::kMergeOpDirectInputPrefix);
  }

  std::vector<ir::Value> GetControlInputs() const {
    return GetVecWithPrefix(merge_operation_->inputs(),
                            DecoderContext::kMergeOpControlInputPrefix);
  }

 private:
  static std::optional<uint64_t> ExtractIdxAfterPrefix(
      absl::string_view str, absl::string_view prefix) {
    if (!absl::StartsWith(str, prefix)) return std::nullopt;
    uint64_t result = 0;
    std::string index_str(str.substr(prefix.size()));
    if (!absl::SimpleAtoi(index_str, &result)) return std::nullopt;
    return result;
  }

  static std::vector<ir::Value> GetVecWithPrefix(ir::NamedValueMap map,
                                                 absl::string_view prefix) {
    uint64_t inferred_vec_length = 0;

    // The numbering of inputs should be dense. Thus, we can find the largest
    // index with the given prefix, add one to it, and consider that the
    // length of the vector.
    for (auto &[key, value] : map) {
      if (std::optional<uint64_t> opt_idx =
              ExtractIdxAfterPrefix(key, prefix)) {
        inferred_vec_length = std::max(inferred_vec_length, *opt_idx + 1);
      }
    }

    // For each item with the prefix up to the inferred length, get the value
    // and put it in the vector. If the numbering is not dense as we expect,
    // fail.
    std::vector<ir::Value> result;
    result.reserve(inferred_vec_length);
    for (uint64_t i = 0; i < inferred_vec_length; ++i) {
      auto find_result = map.find(absl::StrCat(prefix, i));
      CHECK(find_result != map.end())
          << "Found a hole in the key numbering: " << find_result->first;
      result.push_back(find_result->second);
    }
    return result;
  }

  const ir::Operation *merge_operation_;
};

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_MERGE_OPERATION_VIEW_H_
