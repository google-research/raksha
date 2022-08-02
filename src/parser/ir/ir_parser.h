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
#include "src/ir/ssa_names.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrLexer.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrParser.h"

namespace raksha::ir {
using ir_parser_generator::IrLexer;
using ir_parser_generator::IrParser;
using UnresolvedValueMap =
    absl::flat_hash_map<std::string, std::vector<std::string>>;

class IrProgramParser {
 public:
  struct Result {
    std::unique_ptr<IRContext> context;
    std::unique_ptr<Module> module;
    std::unique_ptr<SsaNames> ssa_names;
  };

  struct ConstructOperationResult {
    std::string op_name;
    std::unique_ptr<Operation> operation;
    std::vector<std::string> input_names;
  };

  IrProgramParser::Result ParseProgram(absl::string_view prog_text);
  IrProgramParser()
      : context_(std::make_unique<IRContext>()),
        ssa_names_(std::make_unique<SsaNames>()),
        module_(std::make_unique<Module>()){};

 private:
  ConstructOperationResult ConstructOperation(
      IrParser::OperationContext& operation_context,
      BlockBuilder& block_builder);
  void ConstructBlock(IrParser::BlockContext& block_context);
  void ConstructModule(IrParser::ModuleContext& module_context);
  absl::flat_hash_map<std::string, const Block*> block_map_;
  std::unique_ptr<IRContext> context_;
  std::unique_ptr<SsaNames> ssa_names_;
  std::unique_ptr<Module> module_;
};

}  // namespace raksha::ir

#endif
