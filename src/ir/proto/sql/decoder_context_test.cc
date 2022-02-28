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

TEST(IRContextTest, GetOrCreateStorageIdempotence) {
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

TEST(DecoderContextTest, DecoderContextRegisterAndGetValueTest) {
  IRContext ir_context;
  DecoderContext decoder_context(ir_context);

  Storage storage("storage1", ir_context.type_factory().MakePrimitiveType());

  const Value &value1 = decoder_context.RegisterValue(1, Value(value::Any()));
  EXPECT_THAT(value1.If<value::Any>(), testing::NotNull());
  const Value &value2 =
      decoder_context.RegisterValue(2, Value(value::StoredValue(storage)));
  const Value &get_value1 = decoder_context.GetValue(1);
  const Value &get_value2 = decoder_context.GetValue(2);
  EXPECT_EQ(&get_value1, &value1);
  EXPECT_EQ(&get_value2, &value2);

  EXPECT_DEATH({ decoder_context.RegisterValue(1, Value(value::Any())); },
               "id_to_value map has more than one value associated with the id "
               "1.");
  EXPECT_DEATH({ decoder_context.RegisterValue(2, Value(value::Any())); },
               "id_to_value map has more than one value associated with the id "
               "2.");
  EXPECT_DEATH({ decoder_context.GetValue(3); },
               "Could not find a value with id 3.");
}


}  // namespace raksha::ir::proto::sql
