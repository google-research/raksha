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
#include "src/ir/ir_printer.h"

#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/module.h"
#include "src/ir/value.h"

namespace raksha::ir {
namespace {

class IRPrinterTest : public testing::Test {
 public:
  IRPrinterTest() : plus_op_("core.plus"), minus_op_("core.minus") {
    BlockBuilder builder;
    operation_ = std::addressof(builder.AddOperation(plus_op_, {}, {}));
    block_ = std::addressof(global_module_.AddBlock(builder.build()));
    global_module_.AddBlock(std::make_unique<Block>());
  }

  const Operator& plus_op() const { return plus_op_; }
  const Operator& minus_op() const { return minus_op_; }
  const Block& block() const { return *block_; }
  const Module& global_module() const { return global_module_; }

 private:
  Operator plus_op_;
  Operator minus_op_;

  Module global_module_;
  const Operation* operation_;
  const Block* block_;
};

TEST_F(IRPrinterTest, PrettyPrintsModuleWithProperIndentation) {
  EXPECT_EQ(IRPrinter::ToString(global_module()), R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)");
}

TEST_F(IRPrinterTest, PrettyPrintsBlockWithProperIndentation) {
  EXPECT_EQ(IRPrinter::ToString(block()), R"(block b0 {
  %0 = core.plus []()
}  // block b0
)");
}

TEST_F(IRPrinterTest, PrettyPrintsOperationWithProperIndentation) {
  Operation operation(nullptr, plus_op(), {}, {}, std::make_unique<Module>());
  EXPECT_EQ(IRPrinter::ToString(operation), R"(%0 = core.plus []() {
  module m0 {
  }  // module m0
}
)");
}

TEST_F(IRPrinterTest, PrettyPrintAttributesAndArgsInSortedOrder) {
  Operation operation(
      nullptr, minus_op(),
      {{"access", StringAttribute::Create("private")},
       {"name", StringAttribute::Create("addition")}},
      {{"const", Value(value::Any())}, {"arg", Value(value::Any())}});

  EXPECT_EQ(IRPrinter::ToString(operation),
            "%0 = core.minus [access: private, name: addition](arg: <<ANY>>, "
            "const: <<ANY>>)\n");
}

}  // namespace
}  // namespace raksha::ir
