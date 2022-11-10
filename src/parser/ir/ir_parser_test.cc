//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------
#include "src/parser/ir/ir_parser.h"

#include "src/common/testing/gtest.h"
#include "src/ir/ir_printer.h"

namespace raksha::ir {

class IrParserRoundtripTest : public testing::TestWithParam<absl::string_view> {};

TEST_P(IrParserRoundtripTest, SimpleTestOperation) {
  auto input_program_text = GetParam();
  IrProgramParser ir_parser;
  auto result = ir_parser.ParseProgram(input_program_text);
  EXPECT_EQ(IRPrinter::ToString(*result.module, *result.ssa_names),
            input_program_text);
}

INSTANTIATE_TEST_SUITE_P(IrParserRoundtripTest, IrParserRoundtripTest,
                         ::testing::Values(
                             R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0, %1 = core.plus []()
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: "private", transform: "no"](<<ANY>>, <<ANY>>)
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: "private", transform: "no"](%1, <<ANY>>)
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%3, <<ANY>>)
    %3 = core.mult [access: "private", transform: 45](%3, %2)
    %4 = core.floating_value [value: 8]()
    %5 = core.floating_value [value: 1]()
    %6 = core.mult_floating [access: "private", transform: "no"](%4, %5)
    %4 = core.floating_value [value: 8]()
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%3, <<ANY>>)
    %3 = core.mult [lhs: 10, rhs: "59"](<<ANY>>, %2)
    %4 = core.mult_floating [lhs: 3l, rhs: -0.5l]()
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.select []()
    %2, %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %custom = core.plus [access: "private", transform: "no"](%3, <<ANY>>)
    %3 = core.mult [lhs: 10, rhs: "59"](<<ANY>>, %custom)
    %4, %7 = core.mult_floating [lhs: 3l, rhs: -0.5l]()
  }  // block b1
}  // module m0
)",
                             R"(module m0 {
  block b0 {
    %0 = core.plus [access: "private", transform: "no"](%1, <<ANY>>)
    %1 = core.plus []()
  }  // block b0
  block b1 {
    %0 = core.plus [access: "private", transform: "no"](%2, <<ANY>>)
    %2 = core.plus [access: "private", transform: "no"](%0, <<ANY>>)
  }  // block b1
}  // module m0
)"));

struct ParserInputAndExpectedOutput {
  absl::string_view input;
  absl::string_view output;
};

class IrParserNormalizingTest : public testing::TestWithParam<ParserInputAndExpectedOutput> {};

TEST_P(IrParserNormalizingTest, FloatsAreNormalizedInTestOperations) {
  auto program_text = GetParam();
  IrProgramParser ir_parser;
  auto result = ir_parser.ParseProgram(program_text.input);
  EXPECT_EQ(IRPrinter::ToString(*result.module, *result.ssa_names),
            program_text.output);
}

static const ParserInputAndExpectedOutput kNormalizedIR[] = {
    {
        .input = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
}  // module m0
)",
        .output = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
}  // module m0
)",
    },
    {
        .input = R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%3, <<ANY>>)
    %3 = core.mult [lhs: 10e0, rhs: "59"](<<ANY>>, %2)
    %4 = core.mult_floating [lhs: 3e-2l, rhs: -0.5, other: 1E+100, no_sign_exponent: 0.1e10]()
  }  // block b1
}  // module m0
)",
        .output = R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%3, <<ANY>>)
    %3 = core.mult [lhs: 10l, rhs: "59"](<<ANY>>, %2)
    %4 = core.mult_floating [lhs: 0.03l, no_sign_exponent: 1e+09l, other: 1e+100l, rhs: -0.5l]()
  }  // block b1
}  // module m0
)",
    }};

INSTANTIATE_TEST_SUITE_P(IrParserNormalizingTest, IrParserNormalizingTest,
    ::testing::ValuesIn(kNormalizedIR));

TEST(IrParseTest, ValueNotFoundCausesFailure) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: "private", transform: "no"](%2, <<ANY>>)
  }  // block b1
}  // module m0
)";
  IrProgramParser ir_parser;
  EXPECT_DEATH(ir_parser.ParseProgram(input_program_text), "Value not found");
}

TEST(IrParseTest, ValueAccessedBetweenBlockCausesFailure) {
  auto input_program_text = R"(module m0 {
  block b0 {
     %0 = core.plus []()
  }  // block b0
  block b1 {
      %1 = core.plus [access: "private", transform: "no"](%0, <<ANY>>)
  }  // block b1
}  // module m0
)";
  IrProgramParser ir_parser;
  EXPECT_DEATH(ir_parser.ParseProgram(input_program_text), "Value not found");
}

TEST(IrParseTest, NoStringAttributeQuoteCausesFailure) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: private, transform: "no"](%2, <<ANY>>)
  }  // block b1
}  // module m0
)";

  IrProgramParser ir_parser;
  EXPECT_DEATH(ir_parser.ParseProgram(input_program_text),

               "no viable alternative at input");
}

TEST(IrParseTest, OperatorWithDifferentNumberOfReturnFailure) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %0 = core.make_pair []()
    %1, %2 = core.make_pair [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)";

  IrProgramParser ir_parser;
  EXPECT_DEATH(
      ir_parser.ParseProgram(input_program_text),
      "Operator has different number of return values in different instances ");
}

TEST(IrParserOperationTest, SimpleTestOperationAPIs) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %1, %2 = core.make_pair [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)";
  IrProgramParser ir_parser;
  auto result = ir_parser.ParseProgram(input_program_text);
  EXPECT_EQ(IRPrinter::ToString(*result.module, *result.ssa_names),
            input_program_text);
  auto& op = result.module->blocks()[0]->operations()[0];
  EXPECT_EQ(result.ssa_names->GetOrCreateID(op->GetOutputValue(1)), "%2");
  EXPECT_EQ(result.ssa_names->GetOrCreateID(op->GetOutputValue(0)), "%1");
  EXPECT_EQ(result.ssa_names->GetOrCreateID(op->GetInputValue(0)), "%2");
  EXPECT_EQ(op->NumberOfOutputs(), 2);
  EXPECT_DEATH(result.ssa_names->GetOrCreateID(op->GetOutputValue(2)),
               "Operation:GetOutputValue fails because index is out of bounds");
  EXPECT_DEATH(result.ssa_names->GetOrCreateID(op->GetInputValue(2)),
               "Operation:GetInputValue fails because index is out of bounds");
}
TEST(IrParserOperationDeathTest, DiesIfRequestingOutOfBoundOutputs) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %1, %2 = core.make_pair [](%2, <<ANY>>)
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)";
  IrProgramParser ir_parser;
  auto result = ir_parser.ParseProgram(input_program_text);
  EXPECT_EQ(IRPrinter::ToString(*result.module, *result.ssa_names),
            input_program_text);
  auto& op = result.module->blocks()[0]->operations()[0];
  EXPECT_DEATH(result.ssa_names->GetOrCreateID(op->GetOutputValue(2)),
               "Operation:GetOutputValue fails because index is out of bounds");
}

TEST(IrParserOperationDeathTest, DiesIfRequestingOutOfBoundInputs) {
  auto input_program_text = R"(module m0 {
  block b0 {
    %1, %2 = core.make_pair [](%2, <<ANY>>)
  }  // block b0
  block b1 {
  }  // block b1
}  // module m0
)";
  IrProgramParser ir_parser;
  auto result = ir_parser.ParseProgram(input_program_text);
  EXPECT_EQ(IRPrinter::ToString(*result.module, *result.ssa_names),
            input_program_text);
  auto& op = result.module->blocks()[0]->operations()[0];
  EXPECT_DEATH(result.ssa_names->GetOrCreateID(op->GetInputValue(2)),
               "Operation:GetInputValue fails because index is out of bounds");
}
}  // namespace raksha::ir
