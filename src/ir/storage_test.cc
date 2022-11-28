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
#include "src/ir/storage.h"

#include "src/common/testing/gtest.h"
#include "src/ir/module.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/type.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/value.h"

namespace raksha::ir {
namespace {

using raksha::ir::Value;
using raksha::ir::value::Any;
using raksha::ir::value::OperationResult;
using testing::IsEmpty;
using testing::UnorderedElementsAre;

class StorageTest : public testing::Test {
 protected:
  types::TypeFactory type_factory_;
};

TEST_F(StorageTest, PropertyAccessorsReturnCorrectValues) {
  Storage input_storage("input", type_factory_.MakePrimitiveType());
  EXPECT_EQ(input_storage.name(), "input");
  EXPECT_EQ(input_storage.type().type_base().kind(),
            types::TypeBase::Kind::kPrimitive);

  const types::Schema& schema = type_factory_.RegisterSchema("TestStruct", {});
  Storage output_storage("output", type_factory_.MakeEntityType(schema));
  EXPECT_EQ(output_storage.name(), "output");
  const types::TypeBase& type_base = output_storage.type().type_base();
  ASSERT_EQ(type_base.kind(), types::TypeBase::Kind::kEntity);
  EXPECT_EQ(static_cast<const types::EntityType&>(type_base).schema().name(),
            "TestStruct");
}

TEST_F(StorageTest, ToStringPrintsNameAndType) {
  Storage input_storage("input", type_factory_.MakePrimitiveType());
  EXPECT_EQ(input_storage.ToString(), "store:input:type");
}

TEST_F(StorageTest, AddInputValues) {
  Operator test_operator("test");
  Operation test_operation(nullptr, test_operator, {}, {});
  Value any_value = Value(Any());
  Value operation_result_0 = Value(OperationResult(test_operation, 0));
  Value operation_result_5 = Value(OperationResult(test_operation, 5));
  Storage storage("store1", type_factory_.MakePrimitiveType());

  EXPECT_THAT(storage.input_values(), IsEmpty());
  storage.AddInputValue(any_value);
  storage.AddInputValue(operation_result_0);
  storage.AddInputValue(operation_result_5);
  EXPECT_THAT(
      storage.input_values(),
      UnorderedElementsAre(any_value, operation_result_0, operation_result_5));
}

}  // namespace
}  // namespace raksha::ir
