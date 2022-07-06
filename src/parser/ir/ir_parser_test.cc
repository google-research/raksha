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

class IrParserTest : public testing::TestWithParam<absl::string_view> {};

TEST_P(IrParserTest, SimpleTestOperation) {
  auto input_program_text = GetParam();
  IrProgramParser ir_parser;
  EXPECT_EQ(IRPrinter::ToString(ir_parser.ParseProgram(input_program_text)),
            input_program_text);
}

INSTANTIATE_TEST_SUITE_P(IrParserTest, IrParserTest,
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
    %1 = core.plus [access: "private", transform: "no"](%0.out, <<ANY>>)
  }  // block b1
}  // module m0
)",
R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%0.out, <<ANY>>)
    %3 = core.mult [access: "private", transform: "no"](%0.out, %2.out)
  }  // block b1
}  // module m0
)",
R"(module m0 {
  block b0 {
    %0 = core.select []()
    %1 = core.merge [](<<ANY>>, <<ANY>>)
  }  // block b0
  block b1 {
    %2 = core.plus [access: "private", transform: "no"](%0.out, <<ANY>>)
    %3 = core.mult [lhs: 10, rhs: "_59"](%0.out, %2.out)
  }  // block b1
}  // module m0
)",
R"(module m0 {
  block b0 {
    %0 = core.plus [access: "private", transform: "no"](%1, <<ANY>>)
  }  // block b0
  block b1 {
    %1 = core.plus []()
  }  // block b1
}  // module m0
)"));

TEST(IrParseTest, ValueNotFoundCausesFailure) {
  IrProgramParser ir_parser;
  auto input_program_text = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: "private", transform: "no"](%2, <<ANY>>)
  }  // block b1
}  // module m0
)";
  EXPECT_DEATH(IRPrinter::ToString(ir_parser.ParseProgram(input_program_text)),
              "Value not found");
}

TEST(IrParseTest, NoStringAttributeQuoteCausesFailure) {
  IrProgramParser ir_parser;
  auto input_program_text = R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: private, transform: "no"](%2, <<ANY>>)
  }  // block b1
}  // module m0
)";
  EXPECT_DEATH(IRPrinter::ToString(ir_parser.ParseProgram(input_program_text)),
              "no viable alternative at input");
}
}  // namespace raksha::ir
