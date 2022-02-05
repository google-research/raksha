//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
#include "src/ir/value.h"

#include "src/common/testing/gtest.h"
#include "src/ir/module.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {
namespace {

struct TestData {
 public:
  TestData()
      : plus_operator("+"),
        minus_operator("-"),
        plus_operation(nullptr, plus_operator, {}, {}),
        minus_operation(nullptr, minus_operator, {}, {}),
        input_storage("input", type_factory.MakePrimitiveType()),
        output_storage("output", type_factory.MakePrimitiveType()),
        first_block_arg(first_block, "one"),
        second_block_arg(second_block, "two"),
        plus_operation_result(plus_operation, "plus"),
        minus_operation_result(minus_operation, "minus"),
        input_stored_value(input_storage),
        output_stored_value(output_storage) {
    // Create SSA names to keep tests reproducible.
    ssa_names.GetOrCreateID(first_block);
    ssa_names.GetOrCreateID(second_block);
    ssa_names.GetOrCreateID(plus_operation);
    ssa_names.GetOrCreateID(minus_operation);
  }

  types::TypeFactory type_factory;
  Operator plus_operator;
  Operator minus_operator;
  Block first_block;
  Block second_block;
  Operation plus_operation;
  Operation minus_operation;
  Storage input_storage;
  Storage output_storage;
  value::BlockArgument first_block_arg;
  value::BlockArgument second_block_arg;
  value::OperationResult plus_operation_result;
  value::OperationResult minus_operation_result;
  value::StoredValue input_stored_value;
  value::StoredValue output_stored_value;
  SSANames ssa_names;
};

TestData test_data;

TEST(ValueTest, AccessorsReturnCorrectValue) {
  EXPECT_EQ(&test_data.plus_operation_result.operation(),
            &test_data.plus_operation);
  EXPECT_EQ(test_data.plus_operation_result.name(), "plus");

  EXPECT_EQ(&test_data.minus_operation_result.operation(),
            &test_data.minus_operation);
  EXPECT_EQ(test_data.minus_operation_result.name(), "minus");

  EXPECT_EQ(&test_data.first_block_arg.block(), &test_data.first_block);
  EXPECT_EQ(test_data.first_block_arg.name(), "one");

  EXPECT_EQ(&test_data.second_block_arg.block(), &test_data.second_block);
  EXPECT_EQ(test_data.second_block_arg.name(), "two");

  EXPECT_EQ(&test_data.input_stored_value.storage(), &test_data.input_storage);
  EXPECT_EQ(&test_data.output_stored_value.storage(),
            &test_data.output_storage);
}

class ValueToStringTest
    : public testing::TestWithParam<std::pair<Value, absl::string_view>> {};

TEST_P(ValueToStringTest, ToStringReturnsExpectedFormat) {
  const auto& [value, value_string] = GetParam();
  EXPECT_EQ(value.ToString(test_data.ssa_names), value_string);
}

INSTANTIATE_TEST_SUITE_P(
    ValueToStringTest, ValueToStringTest,
    testing::Values(
        std::make_pair(Value(test_data.first_block_arg), "%0.one"),
        std::make_pair(Value(test_data.second_block_arg), "%1.two"),
        std::make_pair(Value(test_data.plus_operation_result), "%0.plus"),
        std::make_pair(Value(test_data.minus_operation_result), "%1.minus"),
        std::make_pair(Value(test_data.input_stored_value), "store:input:type"),
        std::make_pair(Value(test_data.output_stored_value),
                       "store:output:type"),
        std::make_pair(Value(value::Any()), "<<ANY>>")));

}  // namespace
}  // namespace raksha::ir
