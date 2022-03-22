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
#include "src/ir/ir_context.h"

#include "src/common/testing/gtest.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {
namespace {

TEST(IRContextTest, RegisterOperatorReturnsRegisteredOp) {
  IRContext context;
  const Operator& op =
      context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  EXPECT_EQ(op.name(), "core.choose");
}

TEST(IRContextTest, GetOperatorReturnsRegisteredOperator) {
  IRContext context;
  const Operator& registered_op =
      context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  const Operator* op = context.GetOperator("core.choose");
  ASSERT_NE(op, nullptr);
  EXPECT_EQ(op->name(), "core.choose");
  EXPECT_EQ(op, std::addressof(registered_op));
}

TEST(IRContextTest, IsRegisteredOperatorReturnsCorrectValues) {
  IRContext context;
  context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  EXPECT_TRUE(context.IsRegisteredOperator("core.choose"));
  EXPECT_FALSE(context.IsRegisteredOperator("core.nose"));
}

TEST(IRContextTest, GetOperatorReturnsNullptrForUnregisteredOperator) {
  IRContext context;
  EXPECT_EQ(context.GetOperator("core.choose"), nullptr);
}

TEST(IRContextDeathTest, DuplicateOperatorRegistrationCausesFailure) {
  IRContext context;
  context.RegisterOperator(std::make_unique<Operator>("core.choose"));
  EXPECT_DEATH(
      { context.RegisterOperator(std::make_unique<Operator>("core.choose")); },
      "Cannot register duplicate operator with name 'core.choose'.");
}

TEST(IRContextTest, RegisterStorageReturnsRegisteredStorage) {
  IRContext context;
  types::TypeFactory& type_factory = context.type_factory();

  const Storage& store = context.RegisterStorage(
      std::make_unique<Storage>("store1", type_factory.MakePrimitiveType()));
  EXPECT_EQ(store.name(), "store1");
  EXPECT_EQ(store.type().type_base().kind(), types::TypeBase::Kind::kPrimitive);
}

TEST(IRContextTest, GetStorageReturnsRegisteredStorage) {
  IRContext context;
  types::TypeFactory& type_factory = context.type_factory();

  const Storage& registered_storage = context.RegisterStorage(
      std::make_unique<Storage>("storage1", type_factory.MakePrimitiveType()));
  const Storage* storage = context.GetStorage("storage1");
  ASSERT_NE(storage, nullptr);
  EXPECT_EQ(storage->name(), "storage1");
  EXPECT_EQ(storage->type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);
  EXPECT_EQ(storage, std::addressof(registered_storage));
}

TEST(IRContextTest, IsRegisteredStorageReturnsCorrectValues) {
  IRContext context;
  types::TypeFactory& type_factory = context.type_factory();
  context.RegisterStorage(
      std::make_unique<Storage>("storage1", type_factory.MakePrimitiveType()));
  EXPECT_TRUE(context.IsRegisteredStorage("storage1"));
  EXPECT_FALSE(context.IsRegisteredStorage("storage2"));
}

TEST(IRContextDeathTest, GetStorageReturnsNullptrForUnregisteredStorage) {
  IRContext context;
  EXPECT_EQ(context.GetStorage("not_a_store"), nullptr);
}

TEST(IRContextDeathTest, DuplicateStorageRegistrationCausesFailure) {
  IRContext context;
  types::TypeFactory& type_factory = context.type_factory();
  context.RegisterStorage(
      std::make_unique<Storage>("store", type_factory.MakePrimitiveType()));
  EXPECT_DEATH(
      {
        context.RegisterStorage(std::make_unique<Storage>(
            "store", type_factory.MakePrimitiveType()));
      },
      "Cannot register duplicate storage with name 'store'.");
}

}  // namespace
}  // namespace raksha::ir
