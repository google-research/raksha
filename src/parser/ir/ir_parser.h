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
#include "src/ir/operator.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/value.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrLexer.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrParser.h"

namespace raksha::ir {
using ir_parser_generator::IrLexer;
using ir_parser_generator::IrParser;

class IrProgramParser {
 public:
  // Parses an input string
  Operation ParseProgram(absl::string_view prog_text);

 private:
  Operation ConstructOperation(IrParser::OperationContext& operation_context);
  absl::flat_hash_map<absl::string_view, Operation> operation_map;
  IRContext context;
};

}  // namespace raksha::ir

#endif
