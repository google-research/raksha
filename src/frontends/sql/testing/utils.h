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

}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_UTILS_H_
