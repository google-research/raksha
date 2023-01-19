//-------------------------------------------------------------------------------
// Copyright 2023 Google LLC
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

#include "src/frontends/sqlite/ast.h"

#include <memory>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/die_if_null.h"
#include "src/common/testing/gtest.h"

namespace raksha::frontends::sqlite {
namespace {

TEST(SqliteAstTestSuite, LiteralTest) {
  LiteralValue lit("testval");
  EXPECT_TRUE(lit.value() == "testval");
}

TEST(SqliteAstTestSuite, ExprTest) {
  std::vector<std::unique_ptr<Expr>> exprs;
  exprs.push_back(std::make_unique<LiteralValue>(LiteralValue("testval")));
  exprs.push_back(std::make_unique<LiteralValue>(LiteralValue("testval2")));
  MergeExpr mergeExpr(std::move(exprs));
  EXPECT_EQ(ABSL_DIE_IF_NULL(dynamic_cast<const LiteralValue*>(&mergeExpr[0]))
                ->value(),
            "testval");
  EXPECT_EQ(ABSL_DIE_IF_NULL(dynamic_cast<const LiteralValue*>(&mergeExpr[1]))
                ->value(),
            "testval2");
}

TEST(SqliteAstTestSuite, TableStarTest) {
  TableStar tableStar("test_table");
  EXPECT_EQ(tableStar.table_name(), "test_table");
}

TEST(SqliteAstTestSuite, TableTest) {
  TableName testTable("test_table");
  EXPECT_EQ(testTable.table_name(), "test_table");
}

TEST(SqliteAstTestSuite, StmtTest) {
  std::vector<std::unique_ptr<ResultColumn>> result_columns;
  result_columns.push_back(std::make_unique<TableStar>("test_table_star1"));
  Select select(false, std::move(result_columns),
                std::make_unique<TableName>("test_table"));
  EXPECT_EQ(ABSL_DIE_IF_NULL(dynamic_cast<const TableStar*>(&select[0]))
                ->table_name(),
            "test_table_star1");
  EXPECT_TRUE(select.is_all());
  EXPECT_FALSE(select.is_distinct());
  EXPECT_EQ(
      ABSL_DIE_IF_NULL(dynamic_cast<const TableName*>(&select.input_scan()))
          ->table_name(),
      "test_table");
  std::vector<std::unique_ptr<ResultColumn>> result_columns2;
  Select select2(true, std::move(result_columns2),
                 std::make_unique<TableName>("test_table"));
  EXPECT_TRUE(select2.is_distinct());
  EXPECT_FALSE(select2.is_all());
  EXPECT_EQ(
      ABSL_DIE_IF_NULL(dynamic_cast<const TableName*>(&select2.input_scan()))
          ->table_name(),
      "test_table");
}

TEST(SqliteAstTestSuite, ProgramTest) {
  std::vector<std::unique_ptr<Scan>> scans;
  std::vector<std::unique_ptr<ResultColumn>> result_columns;
  result_columns.push_back(std::make_unique<TableStar>("test_table_star1"));
  scans.push_back(
      std::make_unique<Select>(false, std::move(result_columns),
                               std::make_unique<TableName>("table_name")));
  Program program(std::move(scans));
  const Select& selectStatementGotBack =
      *ABSL_DIE_IF_NULL(dynamic_cast<const Select*>(&program[0]));
  EXPECT_EQ(ABSL_DIE_IF_NULL(dynamic_cast<const TableName*>(
                                 &selectStatementGotBack.input_scan()))
                ->table_name(),
            "table_name");
  EXPECT_EQ(ABSL_DIE_IF_NULL(
                dynamic_cast<const TableStar*>(&selectStatementGotBack[0]))
                ->table_name(),
            "test_table_star1");
  EXPECT_TRUE(selectStatementGotBack.is_all());
  EXPECT_FALSE(selectStatementGotBack.is_distinct());
}

}  // namespace
}  // namespace raksha::frontends::sqlite
