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

#include <tuple>

#include "zetasql/public/parse_helpers.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"

namespace raksha::parser::sql {
namespace {

using testing::TestWithParam;
using testing::ValuesIn;

class SqlParserTest
    : public TestWithParam<std::tuple<absl::string_view, bool>> {};

TEST_P(SqlParserTest, SimpleTest) {
  auto [input_program_text, is_sql] = GetParam();
  auto result = zetasql::IsValidStatementSyntax(
      input_program_text, zetasql::ERROR_MESSAGE_WITH_PAYLOAD);
  EXPECT_EQ(result.ok(), is_sql);
}

constexpr std::tuple<absl::string_view, bool> kTestStatements[] = {
    {"SELECT 5;", true}, {"int foo(int i);", false}};

INSTANTIATE_TEST_SUITE_P(SqlParserTest, SqlParserTest,
                         ValuesIn(kTestStatements));
}  // namespace
}  // namespace raksha::parser::sql
