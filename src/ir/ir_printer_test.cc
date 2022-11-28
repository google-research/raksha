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

#include "absl/strings/string_view.h"
#include "src/common/containers/hash_map.h"
#include "src/common/testing/gtest.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/module.h"
#include "src/ir/value.h"
#include "src/parser/ir/ir_parser.h"
#include "re2/re2.h"

namespace raksha::ir {
namespace {

using testing::TestWithParam;
using testing::ValuesIn;

enum class ToStringVariant {
  // Call IRPrinter::ToString(const Entity&)
  kToString,
  // Call IRPrinter::ToString(std::ostream&, const Entity&)
  kToStringOstream,
  // Call `out << entity`
  kOperator,
};

class IRPrinterTest : public TestWithParam<ToStringVariant> {
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

  template <typename T>
  std::string ToString(const T& entity) {
    switch (GetParam()) {
      case ToStringVariant::kToString:
        return IRPrinter::ToString(entity);
      case ToStringVariant::kToStringOstream: {
        std::ostringstream out;
        IRPrinter::ToString(out, entity);
        return out.str();
      }
      case ToStringVariant::kOperator: {
        std::ostringstream out;
        out << entity;
        return out.str();
      }
    }
    // Placate compiler by marking path as unreachable.
    LOG(FATAL) << "Unreachable!";
    return "";
  }

 private:
  Operator plus_op_;
  Operator minus_op_;

  Module global_module_;
  const Operation* operation_;
  const Block* block_;
};

INSTANTIATE_TEST_SUITE_P(IRPrinterTest, IRPrinterTest,
                         testing::Values(ToStringVariant::kToString,
                                         ToStringVariant::kToStringOstream,
                                         ToStringVariant::kOperator));

TEST_P(IRPrinterTest, PrettyPrintsModuleWithProperIndentation) {
  EXPECT_EQ(ToString(global_module()), R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)");
}

TEST_P(IRPrinterTest, PrettyPrintsBlockWithProperIndentation) {
  EXPECT_EQ(ToString(block()), R"(block b0 {
  %0 = core.plus []()
}  // block b0
)");
}

TEST_P(IRPrinterTest, PrettyPrintsOperationWithProperIndentation) {
  Operation operation(nullptr, plus_op(), {}, {});
  EXPECT_EQ(ToString(operation), R"(%0 = core.plus []()
)");
}

TEST_P(IRPrinterTest, PrettyPrintAttributesInSortedOrder) {
  Operation operation(
      nullptr, minus_op(),
      {{"access", Attribute::Create<StringAttribute>("private")},
       {"name", Attribute::Create<StringAttribute>("addition")}},
      {Value(value::Any()), Value(value::Any())});

  EXPECT_EQ(ToString(operation),
            "%0 = core.minus [access: \"private\", name: "
            "\"addition\"](<<ANY>>, <<ANY>>)\n");
}

TEST(IRPrinterPrintNamedMapOrderTest, PrintsInSortedOrder) {
  common::containers::HashMap<std::string, int> values = {
      {"zip", -1},
      {"hello", 1},
      {"world", 2},
      {"number_42", 42},
  };
  EXPECT_THAT(IRPrinter::PrintNamedMapInNameOrder(
                  values, [](int x) { return absl::StrFormat("%d", x); }),
              "hello: 1, number_42: 42, world: 2, zip: -1");
}

// This test ensures that parsing Raksha IR and printing it out again produces
// the same result. This is to check that the parsing/printing process does not
// distort the SSA value graph (see issue #690). Note that for this test to
// work, values must be assigned to SSA values ordered in the number the appear.
// It would be nice to not have this constraint, but it would require some deep
// work on comparing IR structures that we have not yet done.
class IRParserPrinterRoundTripTest : public TestWithParam<absl::string_view> {};

// Normalize the printed output so we don't have to worry about whitespace
// differences.
std::string EliminateWhitespaceAndComments(std::string s) {
  RE2::GlobalReplace(&s, R"(//.*)", "");
  RE2::GlobalReplace(&s, R"(\s+)", "");
  return s;
}

TEST_P(IRParserPrinterRoundTripTest, IrPrinterRoundTripTest) {
  std::string original_program(GetParam());
  IrProgramParser parser;
  IrProgramParser::Result result = parser.ParseProgram(original_program);
  std::string reprinted = IRPrinter::ToString(*result.module);
  EXPECT_EQ(EliminateWhitespaceAndComments(original_program),
            EliminateWhitespaceAndComments(reprinted));
}

absl::string_view kExampleIrPrograms[] = {
    R"(
module m0 {
  block b0 {
    %0 = simple[]()
  }
}
)",
    R"(
module m0 {
  block b0 {
    %0 = simple[]()
    %1 = simple[]()
    %2 = simple[]()
    %3 = takes_args[](%1, %2)
    %4 = takes_args[](%0, %3)
    %5 = takes_args[](%3, %4)
  }
}
)"};

INSTANTIATE_TEST_SUITE_P(IRParserPrinterRoundTripTest,
                         IRParserPrinterRoundTripTest,
                         ValuesIn(kExampleIrPrograms));

}  // namespace
}  // namespace raksha::ir
