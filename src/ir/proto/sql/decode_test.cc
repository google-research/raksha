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

#include "src/ir/proto/sql/decode.h"

#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/ir/proto/sql/sql_ir.pb.h"

namespace raksha::ir::proto::sql {

class DecodeSourceTableColumnTest :
    public testing::TestWithParam<absl::string_view> {};

TEST_P(DecodeSourceTableColumnTest, DecodeSourceTableColumnTest) {
  absl::string_view column_path = GetParam();
  std::string textproto = absl::StrFormat(R"(column_path: "%s")", column_path);
  SourceTableColumn col_proto;

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      textproto, &col_proto))
      << "Could not decode SourceTableColumn";

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  ir::Value result = DecodeSourceTableColumn(col_proto, decoder_context);
  value::StoredValue stored_value = result.As<value::StoredValue>();
  const Storage &storage = stored_value.storage();
  EXPECT_EQ(storage.name(), column_path);
  EXPECT_EQ(storage.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
}

absl::string_view kSourceColumnPaths[] = {
    "MyTable.col",
    "UserAlias",
    "MySchema.MyTable.col2",
    "MySchema.JoinTable.Table1.col",
};

INSTANTIATE_TEST_SUITE_P(
    DecodeSourceTableColumnTest, DecodeSourceTableColumnTest,
    testing::ValuesIn(kSourceColumnPaths));

class DecodeLiteralTest :
    public testing::TestWithParam<absl::string_view> {};

TEST_P(DecodeLiteralTest, DecodeLiteralTest) {
  absl::string_view literal_str = GetParam();
  std::string textproto = absl::StrFormat(R"(literal_str: "%s")", literal_str);
  Literal literal_proto;

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      textproto, &literal_proto))
      << "Could not decode Literal";

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  ir::Value result = DecodeLiteral(literal_proto, decoder_context);
  value::StoredValue stored_value = result.As<value::StoredValue>();
  const Storage &storage = stored_value.storage();
  EXPECT_EQ(storage.name(), absl::StrCat("literal:", literal_str));
  EXPECT_EQ(storage.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
}

absl::string_view kLiteralStrs[] = {
    "100",
    "0",
    "3.1415",
    "hello world!",
    "null",
};

INSTANTIATE_TEST_SUITE_P(
    DecodeLiteralTest, DecodeLiteralTest, testing::ValuesIn(kLiteralStrs));

}  // namespace raksha::ir::proto::sql
