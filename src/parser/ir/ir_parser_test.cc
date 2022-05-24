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

INSTANTIATE_TEST_SUITE_P(
    IrParserTest, IrParserTest,
    ::testing::Values(
        "%0 = core.plus []()\n",
        "%0 = core.minus [access: private, send: yes](<<ANY>>, <<ANY>>)\n"));

}  // namespace raksha::ir
