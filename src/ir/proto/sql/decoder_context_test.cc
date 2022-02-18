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

#include "src/ir/proto/sql/decoder_context.h"

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::proto::sql {

TEST(DecoderContextTest, GetOrCreateStorageTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  const Storage &store1 = decoder_context.GetOrCreateStorage("s1");
  const Storage &store1_again = decoder_context.GetOrCreateStorage("s1");
  EXPECT_EQ(&store1, &store1_again);

  const Storage &store2 = decoder_context.GetOrCreateStorage("s2");
  const Storage &store3 = decoder_context.GetOrCreateStorage("s3");
  const Storage &store2_again = decoder_context.GetOrCreateStorage("s2");
  const Storage &store3_again = decoder_context.GetOrCreateStorage("s3");

  EXPECT_EQ(&store2, &store2_again);
  EXPECT_EQ(&store3, &store3_again);

  EXPECT_NE(&store1, &store2);
  EXPECT_NE(&store2, &store3);
  EXPECT_NE(&store3, &store1);

  EXPECT_EQ(store1.name(), "s1");
  EXPECT_EQ(store2.name(), "s2");
  EXPECT_EQ(store3.name(), "s3");

  EXPECT_EQ(store1.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
  EXPECT_EQ(store2.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
  EXPECT_EQ(store3.type().type_base().kind(),
          types::TypeBase::Kind::kPrimitive);
}

class ClaimTagOnValueTest : public testing::TestWithParam<
    std::tuple<Value, std::tuple<absl::string_view, bool>>>{};

TEST_P(ClaimTagOnValueTest, ClaimTagOnValueTest) {
  const auto &[value, tag_name_and_is_integrity] = GetParam();
  const auto [tag_name, is_integrity] = tag_name_and_is_integrity;

  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  Value result = decoder_context.ClaimTagOnValue(value, tag_name, is_integrity);

  const value::OperationResult &operation_result =
      result.As<value::OperationResult>();
  const Operation &operation = operation_result.operation();

  std::unique_ptr<Block> global_sql_block =
      decoder_context.FinishGlobalSqlBlock();

  EXPECT_EQ(operation.op().name(), "tag_claim");
  EXPECT_EQ(operation.parent(), global_sql_block.get());
  EXPECT_EQ(operation.attributes().size(), 2);

  {
    auto find_res =
        operation.attributes().find(DecoderContext::kTagKindAttributeName);
    EXPECT_NE(find_res, operation.attributes().end());
    EXPECT_EQ(find_res->second->ToString(),
              (is_integrity) ? DecoderContext::kIntegrityTagAttrValue
                             : DecoderContext::kConfidentialityTagAttrValue);
  }

  {
    auto find_res =
        operation.attributes().find(DecoderContext::kAddedTagAttributeName);
    EXPECT_NE(find_res, operation.attributes().end());
    EXPECT_EQ(find_res->second->ToString(), tag_name);
  }

  EXPECT_EQ(operation.inputs().size(), 1);
  auto find_res =
      operation.inputs().find(DecoderContext::kTagClaimInputValueName);
  ASSERT_NE(find_res, operation.inputs().end());

  // TODO(#337): Once we have a comparison for values, check that the wrapped
  //  value is equivalent to the value passed in.
}

static constexpr absl::string_view kTagNames[] = {
    "tag",
    "ANOTHER_TAG",
    "3",
};

static const Operator kFooOperator("foo");
static const Operator kBarOperator("bar");

static const Operation kFooOperation(nullptr, kFooOperator, {}, {});

static const Value kFooOperationResult(
    value::OperationResult(kFooOperation, "result"));

static const Operation kBarOperation(
    nullptr, kBarOperator, NamedAttributeMap{},
    NamedValueMap{{"val", kFooOperationResult}});

static const Value kBarOperationResult(
    value::OperationResult(kBarOperation, "result"));

static const std::vector<Value> kValues = {
    Value(value::Any()),
    kFooOperationResult,
    kBarOperationResult,
};

INSTANTIATE_TEST_SUITE_P(
    ClaimTagOnValueTest, ClaimTagOnValueTest,
    testing::Combine(
        testing::ValuesIn(kValues),
        testing::Combine(testing::ValuesIn(kTagNames), testing::Bool())));

}  // namespace raksha::ir::proto::sql
