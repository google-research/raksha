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
#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/attribute.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::ir {
namespace {

struct OperationTestData {
  Block* block{};
  const Operator* op{};
  NamedAttributeMap attributes{};
  NamedValueMap values{};
  // We don't have a particular order for attributes and arguments.
  // For now, we keep all possible string representation for the tests
  // and make sure the `ToString` method generates at least on of the
  // given representations.
  absl::flat_hash_set<absl::string_view> string_reps{};
};

class OperationTest : public testing::TestWithParam<OperationTestData> {
 public:
  static Block first_block_;
  static Block second_block_;
  static Operator plus_op;
  static Operator minus_op;

 protected:
  SSANames ssa_names_;
};

Block OperationTest::first_block_;
Block OperationTest::second_block_;
Operator OperationTest::plus_op("core.plus");
Operator OperationTest::minus_op("core.minus");

TEST_P(OperationTest, AccessorsAndToStringReturnCorrectValue) {
  const auto& [block, op, attributes, inputs, string_reps] = GetParam();
  Operation operation(block, *op, attributes, inputs);
  EXPECT_EQ(operation.parent(), block);
  EXPECT_EQ(&operation.op(), op);
  // TODO(336): Enable this when we have a comparator for attributes.
  // EXPECT_EQ(operation.attributes(), attributes);
  // TODO(337): Enable this when we have a comparator for values.
  // EXPECT_EQ(operation.inputs(), inputs);
  EXPECT_THAT(string_reps, testing::Contains(operation.ToString(ssa_names_)));
}

INSTANTIATE_TEST_SUITE_P(
    OperationTest, OperationTest,
    testing::Values(
        OperationTestData({nullptr,
                           &OperationTest::plus_op,
                           {},
                           {},
                           {"%0 = core.plus []()"}}),
        OperationTestData({nullptr,
                           &OperationTest::plus_op,
                           {{"const", Int64Attribute::Create(10)}},
                           {},
                           {"%0 = core.plus [const: 10]()"}}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"predicate", StringAttribute::Create("a && b")}},
                           {},
                           {"%0 = core.minus [predicate: a && b]()"}}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"const1", StringAttribute::Create("a")},
                            {"const2", Int64Attribute::Create(42)}},
                           {},
                           {"%0 = core.minus [const1: a, const2: 42]()",
                            "%0 = core.minus [const2: 42, const1: a]()"}}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"const", Int64Attribute::Create(10)}},
                           {{"arg", Value(value::Any())}},
                           {"%0 = core.minus [const: 10](arg: <<ANY>>)"}}),
        OperationTestData(
            {&OperationTest::first_block_,
             &OperationTest::minus_op,
             {{"const", Int64Attribute::Create(10)}},
             {{"a", Value(value::BlockArgument(OperationTest::first_block_,
                                               "arg0"))},
              {"b", Value(value::BlockArgument(OperationTest::first_block_,
                                               "arg1"))}},
             {"%0 = core.minus [const: 10](a: %0.arg0, b: %0.arg1)",
              "%0 = core.minus [const: 10](b: %0.arg1, a: %0.arg0)"}})));

}  // namespace
}  // namespace raksha::ir
