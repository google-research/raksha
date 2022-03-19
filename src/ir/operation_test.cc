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
#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/ir_printer.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::ir {
namespace {

struct OperationTestData {
  Block* block{};
  const Operator* op{};
  NamedAttributeMap attributes{};
  NamedValueMap values{};
  // The expected string representation.
  absl::string_view string_rep;
};

class OperationTest : public testing::TestWithParam<OperationTestData> {
 public:
  static Block first_block_;
  static Operator plus_op;
  static Operator minus_op;
};

Block OperationTest::first_block_;
Operator OperationTest::plus_op("core.plus");
Operator OperationTest::minus_op("core.minus");

TEST(OperationTest, ConstructorSetsModuleCorrectly) {
  auto module = std::unique_ptr<Module>();
  auto passed_in_module = module.get();
  Operation operation(nullptr, OperationTest::plus_op, {}, {},
                      std::move(module));
  EXPECT_EQ(operation.impl_module(), passed_in_module);
}

TEST_P(OperationTest, AccessorsAndToStringReturnCorrectValue) {
  // Keeping it local so that the operation and first_block always get %0.
  SsaNames ssa_names;
  const auto& [block, op, attributes, inputs, string_reps] = GetParam();
  Operation operation(block, *op, attributes, inputs);
  EXPECT_EQ(operation.impl_module(), nullptr);
  EXPECT_EQ(operation.parent(), block);
  EXPECT_EQ(&operation.op(), op);
  // TODO(336): Enable this when we have a comparator for attributes.
  // EXPECT_EQ(operation.attributes(), attributes);
  // TODO(337): Enable this when we have a comparator for values.
  // EXPECT_EQ(operation.inputs(), inputs);
  EXPECT_THAT(string_reps, testing::Eq(IRPrinter::ToString(operation)));
}

INSTANTIATE_TEST_SUITE_P(
    OperationTest, OperationTest,
    testing::Values(
        OperationTestData({nullptr,
                           &OperationTest::plus_op,
                           {},
                           {},
                           "%0 = core.plus []()\n"}),
        OperationTestData({nullptr,
                           &OperationTest::plus_op,
                           {{"const", Int64Attribute::Create(10)}},
                           {},
                           "%0 = core.plus [const: 10]()\n"}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"predicate", StringAttribute::Create("a && b")}},
                           {},
                           "%0 = core.minus [predicate: a && b]()\n"}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"const1", StringAttribute::Create("a")},
                            {"const2", Int64Attribute::Create(42)}},
                           {},
                           "%0 = core.minus [const1: a, const2: 42]()\n"}),
        OperationTestData({nullptr,
                           &OperationTest::minus_op,
                           {{"const", Int64Attribute::Create(10)}},
                           {{"arg", Value(value::Any())}},
                           "%0 = core.minus [const: 10](arg: <<ANY>>)\n"}),
        OperationTestData(
            {&OperationTest::first_block_,
             &OperationTest::minus_op,
             {{"const", Int64Attribute::Create(10)}},
             {{"a", Value(value::BlockArgument(OperationTest::first_block_,
                                               "arg0"))},
              {"b", Value(value::BlockArgument(OperationTest::first_block_,
                                               "arg1"))}},
             "%0 = core.minus [const: 10](a: %0.arg0, b: %0.arg1)\n"})));

}  // namespace
}  // namespace raksha::ir
