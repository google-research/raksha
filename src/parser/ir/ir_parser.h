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

#include "absl/container/flat_hash_map.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"
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
  const Module& ParseProgram(absl::string_view prog_text);
  SsaNames ssa_names_;

 private:
  void ConstructOperation(IrParser::OperationContext& operation_context,
                          BlockBuilder& block_builder);
  void ConstructBlock(IrParser::BlockContext& block_context);
  void ConstructModule(IrParser::ModuleContext& module_context);
  // As it parses through the program, value_map inserts new entity for
  // each operation Ex: %0 = core.minus []() inserts a new entity {"%0",
  // Operation(nullptr, Operator(core.minus), ..., nullptr )}
  absl::flat_hash_map<std::string, const Value*> value_map_;
  absl::flat_hash_map<std::string, const Block*> block_map_;
  absl::flat_hash_map<std::string, const Module*> module_map_;
  IRContext context_;
  Module module_;
};

}  // namespace raksha::ir

#endif
