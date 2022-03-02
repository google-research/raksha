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

#include "src/frontends/sql/decoder_context.h"

#include "src/common/testing/gtest.h"

namespace raksha::frontends::sql {

using testing::Eq;
using testing::IsEmpty;
using testing::IsNull;
using testing::NotNull;
using testing::Pair;
using testing::ResultOf;
using testing::TestWithParam;
using testing::UnorderedElementsAre;
using testing::Values;

using ir::Attribute;
using ir::IRContext;
using ir::Block;
using ir::Operation;
using ir::Storage;
using ir::Value;
using ir::types::TypeBase;
using ir::value::Any;
using ir::value::OperationResult;
using ir::value::StoredValue;

TEST(DecoderContextTest, BuildTopLevelBlock) {
  IRContext context;
  DecoderContext decoder_context(context);

  const Block &block = decoder_context.BuildTopLevelBlock();
  EXPECT_EQ(block.module(), &decoder_context.global_module());
}

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

  EXPECT_EQ(store1.type().type_base().kind(), TypeBase::Kind::kPrimitive);
  EXPECT_EQ(store2.type().type_base().kind(), TypeBase::Kind::kPrimitive);
}

class LiteralOpTest : public TestWithParam<absl::string_view> {};

TEST_P(LiteralOpTest, MakeLiteralOperationTest) {
  IRContext context;
  DecoderContext decoder_context(context);

  absl::string_view literal_str = GetParam();
  const Operation &lit_op = decoder_context.MakeLiteralOperation(literal_str);

  const Block &top_level_block = decoder_context.BuildTopLevelBlock();

  EXPECT_THAT(lit_op.parent(), &top_level_block);
  EXPECT_THAT(lit_op.impl_module(), IsNull());
  EXPECT_THAT(lit_op.inputs(), IsEmpty());
  EXPECT_EQ(lit_op.op().name(), DecoderContext::kSqlLiteralOpName);

  // Check that "attributes" is exactly "literal_str" associated with the
  // parameter value.
  EXPECT_THAT(
      lit_op.attributes(),
      UnorderedElementsAre(
          Pair(DecoderContext::kLiteralStrAttrName,
               ResultOf([](const Attribute &attr) { return attr->ToString(); },
                        Eq(literal_str)))));
}

INSTANTIATE_TEST_SUITE_P(LiteralOpTest, LiteralOpTest,
                         Values("val1", "1000", "3.1415", "true", ""));

TEST(DecoderContextTest, DecoderContextRegisterAndGetValueTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  Storage storage("storage1", ir_context.type_factory().MakePrimitiveType());

  const Value &value1 = decoder_context.RegisterValue(1, Value(Any()));
  EXPECT_THAT(value1.If<Any>(), NotNull());
  const Value &value2 =
      decoder_context.RegisterValue(2, Value(StoredValue(storage)));
  const Value &get_value1 = decoder_context.GetValue(1);
  const Value &get_value2 = decoder_context.GetValue(2);
  EXPECT_EQ(&get_value1, &value1);
  EXPECT_EQ(&get_value2, &value2);
}

TEST(DecoderContextDeathTest, DecoderContextValueRegistryDeathTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  decoder_context.RegisterValue(1, Value(Any()));
  decoder_context.RegisterValue(2, Value(Any()));

  EXPECT_DEATH({ decoder_context.RegisterValue(1, Value(Any())); },
               "id_to_value map has more than one value associated with the id "
               "1.");
  EXPECT_DEATH({ decoder_context.RegisterValue(2, Value(Any())); },
               "id_to_value map has more than one value associated with the id "
               "2.");
  EXPECT_DEATH({ decoder_context.GetValue(3); },
               "Could not find a value with id 3.");
}

}  // namespace raksha::frontends::sql
