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
        first_block_first_arg(first_block, "one"),
        first_block_second_arg(first_block, "two"),
        second_block_first_arg(second_block, "one"),
        second_block_second_arg(second_block, "two"),
        plus_operation_result1(plus_operation, "res1"),
        plus_operation_result2(plus_operation, "res2"),
        minus_operation_result1(minus_operation, "res1"),
        minus_operation_result2(minus_operation, "res2"),
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
  value::BlockArgument first_block_first_arg;
  value::BlockArgument first_block_second_arg;
  value::BlockArgument second_block_first_arg;
  value::BlockArgument second_block_second_arg;
  value::OperationResult plus_operation_result1;
  value::OperationResult plus_operation_result2;
  value::OperationResult minus_operation_result1;
  value::OperationResult minus_operation_result2;
  value::StoredValue input_stored_value;
  value::StoredValue output_stored_value;
  SsaNames ssa_names;
};

TestData test_data;

TEST(ValueTest, AccessorsReturnCorrectValue) {
  EXPECT_EQ(&test_data.plus_operation_result1.operation(),
            &test_data.plus_operation);
  EXPECT_EQ(test_data.plus_operation_result1.name(), "res1");

  EXPECT_EQ(&test_data.plus_operation_result2.operation(),
            &test_data.plus_operation);
  EXPECT_EQ(test_data.plus_operation_result2.name(), "res2");

  EXPECT_EQ(&test_data.minus_operation_result1.operation(),
            &test_data.minus_operation);
  EXPECT_EQ(test_data.minus_operation_result1.name(), "res1");

  EXPECT_EQ(&test_data.minus_operation_result2.operation(),
            &test_data.minus_operation);
  EXPECT_EQ(test_data.minus_operation_result2.name(), "res2");

  EXPECT_EQ(&test_data.first_block_first_arg.block(), &test_data.first_block);
  EXPECT_EQ(test_data.first_block_first_arg.name(), "one");

  EXPECT_EQ(&test_data.first_block_second_arg.block(), &test_data.first_block);
  EXPECT_EQ(test_data.first_block_second_arg.name(), "two");

  EXPECT_EQ(&test_data.second_block_first_arg.block(), &test_data.second_block);
  EXPECT_EQ(test_data.second_block_first_arg.name(), "one");

  EXPECT_EQ(&test_data.second_block_second_arg.block(),
            &test_data.second_block);
  EXPECT_EQ(test_data.second_block_second_arg.name(), "two");

  EXPECT_EQ(&test_data.input_stored_value.storage(), &test_data.input_storage);
  EXPECT_EQ(&test_data.output_stored_value.storage(),
            &test_data.output_storage);
}

class ValueToStringTest
    : public testing::TestWithParam<std::pair<Value, absl::string_view>> {};

TEST_P(ValueToStringTest, ToStringReturnsExpectedFormat) {
  const auto &[value, value_string] = GetParam();
  EXPECT_EQ(value.ToString(test_data.ssa_names), value_string);
}

INSTANTIATE_TEST_SUITE_P(
    ValueToStringTest, ValueToStringTest,
    testing::Values(
        std::make_pair(Value(test_data.first_block_first_arg), "%0.one"),
        std::make_pair(Value(test_data.first_block_second_arg), "%0.two"),
        std::make_pair(Value(test_data.second_block_first_arg), "%1.one"),
        std::make_pair(Value(test_data.second_block_second_arg), "%1.two"),
        std::make_pair(Value(test_data.plus_operation_result1), "%0.res1"),
        std::make_pair(Value(test_data.plus_operation_result2), "%0.res2"),
        std::make_pair(Value(test_data.minus_operation_result1), "%1.res1"),
        std::make_pair(Value(test_data.minus_operation_result2), "%1.res2"),
        std::make_pair(Value(test_data.input_stored_value), "store:input:type"),
        std::make_pair(Value(test_data.output_stored_value),
                       "store:output:type"),
        std::make_pair(Value(value::Any()), "<<ANY>>")));

static std::pair<Value, uint64_t> kSampleValues[] = {
    {Value(test_data.first_block_first_arg), 0},
    {Value(test_data.first_block_second_arg), 1},
    {Value(test_data.second_block_first_arg), 2},
    {Value(test_data.second_block_second_arg), 3},
    {Value(test_data.plus_operation_result1), 4},
    {Value(test_data.plus_operation_result2), 5},
    {Value(test_data.minus_operation_result1), 6},
    {Value(test_data.minus_operation_result2), 7},
    {Value(test_data.input_stored_value), 8},
    {Value(test_data.output_stored_value), 9},
    {Value(value::Any()), 10},
};

class ValueEqTest
    : public testing::TestWithParam<
          std::tuple<std::pair<Value, uint64_t>, std::pair<Value, uint64_t>>> {
};

TEST_P(ValueEqTest, ValueEqTest) {
  auto &[value_and_eq_class1, value_and_eq_class2] = GetParam();
  auto &[value1, eq_class1] = value_and_eq_class1;
  auto &[value2, eq_class2] = value_and_eq_class2;

  EXPECT_EQ(value1 == value2, eq_class1 == eq_class2);
}

INSTANTIATE_TEST_SUITE_P(ValueEqTest, ValueEqTest,
                         testing::Combine(testing::ValuesIn(kSampleValues),
                                          testing::ValuesIn(kSampleValues)));

}  // namespace
}  // namespace raksha::ir
