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
TEST(IrParserTest, SimpleOperation) {
  IrProgramParser ir_parser;
  absl::string_view input_program_text =
      "%0 = core.minus [access: private, name: addition](<<ANY>>, <<ANY>>)\n";
  EXPECT_EQ(IRPrinter::ToString(ir_parser.ParseProgram(input_program_text)),
            input_program_text);
}
}  // namespace raksha::ir
