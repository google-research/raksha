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

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
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

void IrProgramParser::ConstructOperation(
    IrParser::OperationContext& operation_context,
    BlockBuilder& block_builder) {
  // Operator
  if (context_->GetOperator(operation_context.ID()->getText()) == nullptr) {
    context_->RegisterOperator(
        std::make_unique<Operator>(operation_context.ID()->getText()));
  }
  // Grammar rule for attributes is either
  //   -> ID ':' NUMLITERAL (a number type) or
  //   -> ID ':' '"'ID'"' (a string type)
  NamedAttributeMap attributes;
  if (operation_context.attributeList() != nullptr) {
    for (IrParser::AttributeContext* attribute_context :
         operation_context.attributeList()->attribute()) {
      if (auto* string_attribute_context =
              dynamic_cast<IrParser::StringAttributeContext*>(
                  attribute_context)) {
        attributes.insert(
            {CHECK_NOTNULL(string_attribute_context)->ID(0)->getText(),
             Attribute::Create<StringAttribute>(
                 CHECK_NOTNULL(string_attribute_context)->ID(1)->getText())});
        continue;
      }
      auto* num_attribute_context =
          dynamic_cast<IrParser::NumAttributeContext*>(attribute_context);
      int64_t parsed_int = 0;
      bool conversion_succeeds = absl::SimpleAtoi(
          CHECK_NOTNULL(num_attribute_context)->NUMLITERAL()->getText(),
          &parsed_int);
      CHECK(conversion_succeeds == true);
      attributes.insert({CHECK_NOTNULL(num_attribute_context)->ID()->getText(),
                         Attribute::Create<Int64Attribute>(parsed_int)});
    }
  }
  // Grammar rule for value is one of the 2 options below
  //->ANY
  //->VALUE_ID (a stored value or a operation result or a block argument)
  ValueList inputs =
      (operation_context.argumentList() == nullptr)
          ? ValueList()
          : utils::MapIter<Value>(
                operation_context.argumentList()->value(),
                [this](IrParser::ValueContext* value_context) {
                  if (dynamic_cast<IrParser::AnyValueContext*>(value_context)) {
                    return Value(value::Any());
                  }
                  auto* named_value_context =
                      dynamic_cast<IrParser::NamedValueContext*>(value_context);
                  const std::string& value_id =
                      CHECK_NOTNULL(named_value_context)->VALUE_ID()->getText();
                  // Operation Result Value
                  auto find_value_id_operation = value_map_.find(value_id);
                  CHECK(find_value_id_operation != value_map_.end())
                      << "Value not found" << value_id;
                  return find_value_id_operation->second;
                });
  // Operation rule: VALUE_ID '=' ID '['(attributeList)?']''('(argumentList)?')'
  // mapping between Result(Ex:%0) and corresponding operation.
  const Operation& op = block_builder.AddOperation(
      *CHECK_NOTNULL(context_->GetOperator(operation_context.ID()->getText())),
      std::move(attributes), std::move(inputs), nullptr);
  const Value& v = Value(value::OperationResult(op, "out"));
  value_map_.insert({absl::StrCat(operation_context.VALUE_ID()->getText()), v});
  ssa_names_->UpdateID(v, operation_context.VALUE_ID()->getText());
  ssa_names_->UpdateID(op, operation_context.VALUE_ID()->getText());
}

void IrProgramParser::ConstructBlock(IrParser::BlockContext& block_context) {
  BlockBuilder builder;
  for (IrParser::OperationContext* operation_context :
       block_context.operation()) {
    IrProgramParser::ConstructOperation(*CHECK_NOTNULL(operation_context),
                                        builder);
  }
  const Block& b = module_->AddBlock(builder.build());
  block_map_.insert({block_context.ID()->getText(), std::addressof(b)});
  ssa_names_->UpdateID(b, block_context.ID()->getText());
}

void IrProgramParser::ConstructModule(IrParser::ModuleContext& module_context) {
  for (IrParser::BlockContext* block_context : module_context.block()) {
    IrProgramParser::ConstructBlock(*CHECK_NOTNULL(block_context));
  }
  ssa_names_->UpdateID(*module_, module_context.ID()->getText());
}

/// This function produces an abstract syntax tree (AST) rooted with a
/// program node when given the textual representation of a program.
IrProgramParser::Result IrProgramParser::ParseProgram(
    absl::string_view prog_text) {
  // Provide the input text in a stream
  antlr4::ANTLRInputStream input(prog_text);
  // Creates a lexer from input
  IrLexer lexer(&input);
  // Creates a token stream from lexer
  antlr4::CommonTokenStream tokens(&lexer);
  // Creates a parser from the token stream
  IrParser parser(&tokens);
  // program_context points to the root of the parse tree
  IrParser::IrProgramContext& program_context =
      *CHECK_NOTNULL(parser.irProgram());

  IrProgramParser::ConstructModule(*CHECK_NOTNULL(program_context.module()));

  return IrProgramParser::Result{.context = std::move(context_),
                                 .module = std::move(module_),
                                 .ssa_names = std::move(ssa_names_)};
}

}  // namespace raksha::ir
