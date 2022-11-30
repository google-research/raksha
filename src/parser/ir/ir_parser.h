/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.

#ifndef SRC_PARSER_IR_IR_PARSER_H_
#define SRC_PARSER_IR_IR_PARSER_H_

#include <string>

#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/ssa_names.h"

namespace raksha::parser::ir {

struct IrProgramParserResult {
  std::unique_ptr<raksha::ir::IRContext> context;
  std::unique_ptr<raksha::ir::Module> module;
  std::unique_ptr<raksha::ir::SsaNames> ssa_names;
};

IrProgramParserResult ParseProgram(absl::string_view prog_text);

}  // namespace raksha::parser::ir

#endif  // SRC_PARSER_IR_IR_PARSER_H_
