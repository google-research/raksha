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

// Don't seed `Storage`s at static scope for now because the `Storage`s require
// a `Type`, which is a `unique_ptr` to a `TypeBase` only constructable by a
// `TypeFactory`. That's just hard to construct at static scope, so we don't do
// that for now.
//
// We could test seeding both `Storage`s and `Operation`s at once with a
// fixture, but we really want to show that seeding an `IRContext` with
// `Operator`s works at the static scope.
const IRContext kSeededContext({Operator("core.op1"), Operator("core.op2")},
                               {});

TEST(IRContextTest, SeededOperatorsAreRegistered) {
  EXPECT_TRUE(kSeededContext.IsRegisteredOperator("core.op1"));
  EXPECT_TRUE(kSeededContext.IsRegisteredOperator("core.op2"));
  EXPECT_FALSE(kSeededContext.IsRegisteredOperator("core.op3"));
}

TEST(IRContextTest, GetSeededOperators) {
  auto get_op1_result = kSeededContext.GetOperator("core.op1");
  EXPECT_THAT(get_op1_result, testing::NotNull());
  EXPECT_EQ(get_op1_result->name(), "core.op1");

  auto get_op2_result = kSeededContext.GetOperator("core.op2");
  EXPECT_THAT(get_op2_result, testing::NotNull());
  EXPECT_EQ(get_op2_result->name(), "core.op2");

  EXPECT_THAT(kSeededContext.GetOperator("core.op3"), testing::IsNull());
}

TEST(IRContextTest, DoubleRegisterSeededOperatorsFails) {
  IRContext seeded_context({Operator("core.op1"), Operator("core.op2")}, {});
  EXPECT_DEATH(
      {
        seeded_context.RegisterOperator(std::make_unique<Operator>("core.op1"));
      },
      "Cannot register duplicate operator with name 'core.op1'.");
  EXPECT_DEATH(
      {
        seeded_context.RegisterOperator(std::make_unique<Operator>("core.op2"));
      },
      "Cannot register duplicate operator with name 'core.op2'.");
  EXPECT_EQ(
      seeded_context.RegisterOperator(std::make_unique<Operator>("core.op3"))
          .name(),
      "core.op3");
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

class SeededStorageTest : public testing::Test {
 public:
  SeededStorageTest()
      : type_factory_(),
        init_array_{Storage("storage1", type_factory_.MakePrimitiveType()),
                    Storage("storage2", type_factory_.MakePrimitiveType())},
        context_({}, std::vector<Storage>(
                         std::make_move_iterator(std::begin(init_array_)),
                         std::make_move_iterator(std::end(init_array_)))) {}

 protected:
  types::TypeFactory type_factory_;
  // This is a hack to get around the fact that you can only copy out of C++
  // initializer lists. We create a `std::array` to act as a mutable
  // "initializer list" and move items from that into our destination vector.
  std::array<ir::Storage, 2> init_array_;
  IRContext context_;
};

TEST_F(SeededStorageTest, SeededStoragesAreRegistered) {
  EXPECT_TRUE(context_.IsRegisteredStorage("storage1"));
  EXPECT_TRUE(context_.IsRegisteredStorage("storage2"));
  EXPECT_FALSE(context_.IsRegisteredStorage("storage3"));
}

TEST_F(SeededStorageTest, GetSeededStorages) {
  const Storage* storage1 = context_.GetStorage("storage1");
  EXPECT_THAT(storage1, testing::NotNull());
  EXPECT_EQ(storage1->name(), "storage1");

  const Storage* storage2 = context_.GetStorage("storage2");
  EXPECT_THAT(storage2, testing::NotNull());
  EXPECT_EQ(storage2->name(), "storage2");

  EXPECT_THAT(context_.GetStorage("storage3"), testing::IsNull());
}

TEST_F(SeededStorageTest, DoubleRegisterSeededStorages) {
  EXPECT_DEATH(
      {
        context_.RegisterStorage(std::make_unique<Storage>(
            "storage1", type_factory_.MakePrimitiveType()));
      },
      "Cannot register duplicate storage with name 'storage1'.");
  EXPECT_DEATH(
      {
        context_.RegisterStorage(std::make_unique<Storage>(
            "storage2", type_factory_.MakePrimitiveType()));
      },
      "Cannot register duplicate storage with name 'storage2'.");
  EXPECT_EQ(context_
                .RegisterStorage(std::make_unique<Storage>(
                    "storage3", type_factory_.MakePrimitiveType()))
                .name(),
            "storage3");
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
