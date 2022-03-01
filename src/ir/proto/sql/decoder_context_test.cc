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

#include "src/common/testing/gtest.h"

namespace raksha::ir::proto::sql {

TEST(DecoderContextTest, GetOrCreateStorageIdempotence) {
  IRContext context;
  DecoderContext decoder_context(context);
  const Storage &store1 = decoder_context.GetOrCreateStorage("Table1");
  const Storage &store2 = decoder_context.GetOrCreateStorage("Disk1");
  const Storage &store1_again = decoder_context.GetOrCreateStorage("Table1");
  const Storage &store2_again = decoder_context.GetOrCreateStorage("Disk1");

  EXPECT_EQ(&store1, &store1_again);
  EXPECT_EQ(&store2, &store2_again);
  EXPECT_NE(&store1, &store2);

  EXPECT_EQ(store1.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
  EXPECT_EQ(store2.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
}

class LiteralOpTest : public testing::TestWithParam<absl::string_view> {};

TEST_P(LiteralOpTest, MakeLiteralOperationTest) {
  IRContext context;
  DecoderContext decoder_context(context);

  absl::string_view literal_str = GetParam();
  const Operation &lit_op = decoder_context.MakeLiteralOperation(literal_str);

  EXPECT_THAT(lit_op.parent(), testing::IsNull());
  EXPECT_THAT(lit_op.impl_module(), testing::IsNull());
  EXPECT_THAT(lit_op.inputs(), testing::IsEmpty());
  EXPECT_EQ(lit_op.op().name(), "sql.literal");

  // Check that "attributes" is exactly "literal_str" associated with the
  // parameter value.
  const NamedAttributeMap &attributes = lit_op.attributes();
  EXPECT_EQ(attributes.size(), 1);
  auto find_result = attributes.find("literal_str");
  EXPECT_NE(find_result, attributes.end());
  EXPECT_EQ(find_result->second->ToString(), literal_str);
}

INSTANTIATE_TEST_SUITE_P(
    LiteralOpTest, LiteralOpTest,
    testing::Values("val1", "1000", "3.1415", "true", ""));

}  // namespace raksha::ir::proto::sql
