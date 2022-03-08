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
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/id_name_and_string_test.h"

namespace raksha::frontends::sql {

namespace {

using ir::Attribute;
using ir::Block;
using ir::IRContext;
using ir::Operation;
using ir::Storage;
using ir::Value;
using ir::types::TypeBase;
using ir::value::OperationResult;
using ir::value::StoredValue;
using ::testing::Combine;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::ResultOf;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using ::testing::Values;
using ::testing::ValuesIn;

class DecodeSourceTableColumnExprTest : public IdNameAndStringTest {
  absl::ParsedFormat<'u', 's', 's'> GetTextprotoFormat() const override {
    return absl::ParsedFormat<'u', 's', 's'>(
        R"(id: %u %s source_table_column: { column_path: "%s" })");
  }
};

constexpr uint64_t kSampleIds[] = {1, 5, 1000};

constexpr std::optional<absl::string_view> kSampleExprNames[] = {
    {}, {"name1"}, {"another_name"}};

absl::string_view kStrings[] = {"MyTable.col",
                                "UserAlias",
                                "MySchema.MyTable.col2",
                                "MySchema.JoinTable.Table1.col",
                                "9",
                                "hello world!",
                                "3.1415"};

TEST_P(DecodeSourceTableColumnExprTest, DecodeSourceTableColumnExprTest) {
  auto &[id, name, str] = GetParam();
  Value result = GetDecodedValue();
  const StoredValue *stored_value = result.If<StoredValue>();
  EXPECT_THAT(stored_value, NotNull());
  const Storage &storage = stored_value->storage();
  EXPECT_EQ(storage.name(), str);
  EXPECT_EQ(storage.type().type_base().kind(), TypeBase::Kind::kPrimitive);
  EXPECT_EQ(result, decoder_context_.GetValue(id));
}

INSTANTIATE_TEST_SUITE_P(DecodeSourceTableColumnExprTest,
                         DecodeSourceTableColumnExprTest,
                         Combine(ValuesIn(kSampleIds),
                                 ValuesIn(kSampleExprNames),
                                 ValuesIn(kStrings)));

}  // anonymous namespace

}  // namespace raksha::frontends::sql
