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

#ifndef SRC_FRONTENDS_SQL_TESTING_UTILS_H_
#define SRC_FRONTENDS_SQL_TESTING_UTILS_H_

#include "src/frontends/sql/decoder_context.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql::testing {

const ir::Operation &UnwrapDefaultOperationResult(
    ir::Value expected_operation_result) {
  const ir::value::OperationResult *result =
      expected_operation_result.If<ir::value::OperationResult>();
  CHECK(result != nullptr) << "Expected value to be an `OperationResult`.";
  CHECK(result->name() == DecoderContext::kDefaultOutputName)
      << "Expected the result to have the default output name.";
  return result->operation();
}

// Creates an `ExpressionArena` with some number of `Literal` expressions as its
// prefix. Useful for when you need to test that some textproto points at other
// expressions, but you don't care what they are really are.
std::string CreateExprArenaTextprotoWithLiteralsPrefix(
    std::string top_level_proto, std::vector<uint64_t> ids_to_be_filled) {
  const absl::string_view kExprArenaFormat = "id_expression_pairs: [ %s ]";
  // Generate a bunch of literals to act as potential child expressions.
  std::vector<std::string> expr_protos;
  expr_protos.reserve(ids_to_be_filled.size() + 1);
  for (uint64_t id : ids_to_be_filled) {
    expr_protos.push_back(absl::StrFormat(
        R"({ id: %u expression: { literal: { literal_str: "%u" } } })", id,
        id));
  }
  expr_protos.push_back(top_level_proto);
  return absl::StrFormat(kExprArenaFormat, absl::StrJoin(expr_protos, ", "));
}

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_UTILS_H_
