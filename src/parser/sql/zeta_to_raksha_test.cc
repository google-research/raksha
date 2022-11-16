//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include "src/parser/sql/zeta_to_raksha.h"

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"

namespace raksha::parser::sql {
namespace {
TEST(AnalysisResultTest, SimpleTest) {
  EXPECT_EQ(PrintParseResult("SELECT 1;"),
            R"""(QueryStmt
+-output_column_list=
| +-$query.$col1#1 AS `$col1` [INT64]
+-query=
  +-ProjectScan
    +-column_list=[$query.$col1#1]
    +-expr_list=
    | +-$col1#1 := Literal(type=INT64, value=1)
    +-input_scan=
      +-SingleRowScan
)""");
}
}  // namespace
}  // namespace raksha::parser::sql