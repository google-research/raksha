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

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/name_and_string_test.h"
#include "src/frontends/sql/ops/literal_op.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

using ir::Block;
using ir::Operation;
using ir::Value;
using ::testing::Combine;
using ::testing::IsNull;
using ::testing::ValuesIn;

constexpr std::optional<absl::string_view> kSampleExprNames[] = {
    {}, {"name1"}, {"another_name"}};

absl::string_view kStrings[] = {"MyTable.col",
                                "UserAlias",
                                "MySchema.MyTable.col2",
                                "MySchema.JoinTable.Table1.col",
                                "9",
                                "hello world!",
                                "3.1415"};

class DecodeLiteralExprTest : public NameAndStringTest {
  absl::ParsedFormat<'s', 's'> GetTextprotoFormat() const override {
    return absl::ParsedFormat<'s', 's'>(
        R"({ %s literal: { literal_str: "%s" } })");
  }
};

TEST_P(DecodeLiteralExprTest, DecodeLiteralExprTest) {
  auto &[name, str] = GetParam();
  const Value &result = GetDecodedValue();
  const Block &top_block = decoder_context_.BuildTopLevelBlock();

  // Set up finished, now check expectations.
  const Operation &operation = testing::UnwrapDefaultOperationResult(result);
  EXPECT_EQ(operation.parent(), &top_block);
  EXPECT_THAT(operation.impl_module(), IsNull());

  const LiteralOp *literal_op = SqlOp::GetIf<LiteralOp>(operation);
  ASSERT_NE(literal_op, nullptr);
  EXPECT_EQ(literal_op->GetLiteralString(), str);
}

INSTANTIATE_TEST_SUITE_P(DecodeLiteralExprTest, DecodeLiteralExprTest,
                         Combine(ValuesIn(kSampleExprNames),
                                 ValuesIn(kStrings)));

}  // anonymous namespace

}  // namespace raksha::frontends::sql
