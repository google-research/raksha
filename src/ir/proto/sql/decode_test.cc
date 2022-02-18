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

class ComparableSourceTableColumn {
 public:
  ComparableSourceTableColumn() = default;
  ComparableSourceTableColumn(const SourceTableColumn &source_table_column)
    : column_(source_table_column.column_path()),
      confidentiality_tags_(source_table_column.confidentiality_tags().begin(),
                            source_table_column.confidentiality_tags().end()),
      integrity_tags_(source_table_column.integrity_tags().begin(),
                      source_table_column.integrity_tags().end())
      {}

  void AddConfidentialityTag(absl::string_view tag) {
    confidentiality_tags_.insert(std::string(tag));
  }

  void AddIntegrityTag(absl::string_view tag) {
    integrity_tags_.insert(std::string(tag));
  }

  void SetColumn(absl::string_view column) {
    CHECK(!column_) << "Attempt to set a column that was already set!";
    column_ = column;
  }

  void ExpectEq(const ComparableSourceTableColumn &other) const {
    EXPECT_EQ(column_, other.column_);
    EXPECT_EQ(confidentiality_tags_, other.confidentiality_tags_);
    EXPECT_EQ(integrity_tags_, other.integrity_tags_);
  }

 private:
  std::optional<std::string> column_;
  absl::flat_hash_set<std::string> confidentiality_tags_;
  absl::flat_hash_set<std::string> integrity_tags_;
};

// Why are we creating this recursive traversal through the values rather
// than a visitor? The visitor moves through the code of the IR, visiting
// Operations, the Modules implementing those Operations, etc. But the
// properties that we want to verify about the nesting of the claims around a
// column storage are properties of the Values. In general, traversing values
// can get quite complicated, as different outputs of an Operation can be
// sent to different locations and can form loops. Visiting values like this,
// in fact, starts to look like a dataflow analysis, which we want Souffle to
// do. Rather than wrestling with the whole problem now, we take advantage of
// the fact that we expect a single, linear nesting of values which can easily
// be traversed right here.
//
// In addition, the process of traversing the value tree largely overlaps
// with checking the properties that we want to check anyway.
//
// NOTE: It would be nicer to return the result rather than use an output
// parameter, but that would preclude using the `ASSERT` macros (which expect
// to be in a function returning void).
void GatherSourceColumnInfoFromValues(
    const Value val, ComparableSourceTableColumn &result) {
  if (const value::StoredValue *storage_val = val.If<value::StoredValue>()) {
    result.SetColumn(storage_val->storage().name());
  } else if (const value::OperationResult *op_val_ptr =
      val.If<value::OperationResult>()) {
    const Operation &op = op_val_ptr->operation();
    EXPECT_EQ(op.op().name(), DecoderContext::kTagClaimOperatorName);
    auto kind_find_res =
        op.attributes().find(DecoderContext::kTagKindAttributeName);
    std::string tag_kind_string = kind_find_res->second->ToString();
    CHECK((tag_kind_string == DecoderContext::kIntegrityTagAttrValue) ||
      (tag_kind_string == DecoderContext::kConfidentialityTagAttrValue));
    bool is_integrity =
        (tag_kind_string == DecoderContext::kIntegrityTagAttrValue);
    ASSERT_NE(kind_find_res, op.attributes().end());
    auto tag_name_find_res =
        op.attributes().find(DecoderContext::kAddedTagAttributeName);
    ASSERT_NE(tag_name_find_res, op.attributes().end());
    std::string tag_name_string = tag_name_find_res->second->ToString();
    if (is_integrity) {
      result.AddIntegrityTag(tag_name_string);
    } else {
      result.AddConfidentialityTag(tag_name_string);
    }

    EXPECT_EQ(op.inputs().size(), 1);
    auto find_res = op.inputs().find(DecoderContext::kTagClaimInputValueName);
    ASSERT_NE(find_res, op.inputs().end());
    GatherSourceColumnInfoFromValues(find_res->second, result);
  }
}

class DecodeSourceTableColumnTest :
    public testing::TestWithParam<absl::string_view> {};

TEST_P(DecodeSourceTableColumnTest, DecodeSourceTableColumnTest) {
  absl::string_view textproto = GetParam();
  SourceTableColumn col_proto;

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      std::string(textproto), &col_proto))
      << "Could not decode SourceTableColumn";

  ComparableSourceTableColumn expected(col_proto);
  IRContext ir_context;
  DecoderContext ctxt(ir_context);
  ir::Value result = DecodeSourceTableColumn(col_proto, ctxt);
  ComparableSourceTableColumn actual;
  GatherSourceColumnInfoFromValues(result, actual);
  actual.ExpectEq(expected);
}

absl::string_view kSourceColumnTextprotos[] = {
    {R"(column_path: "MyTable.col")"},
    {R"(column_path: "MyTable.col"
        confidentiality_tags: [ "tag1" ])"},
    {R"(column_path: "MyTable.col"
        integrity_tags: [ "tag1" ])"},
    {R"(column_path: "MyTable.col"
        confidentiality_tags: [ "cTag" ]
        integrity_tags: [ "iTag" ])"},
    {R"(column_path: "MyTable.col"
        confidentiality_tags: [ "cTag1", "cTag2" ]
        integrity_tags: [ "iTag1", "iTag2" ])"},
};

INSTANTIATE_TEST_SUITE_P(
    DecodeSourceTableColumnTest, DecodeSourceTableColumnTest,
    testing::ValuesIn(kSourceColumnTextprotos));

}  // namespace raksha::ir::proto::sql
