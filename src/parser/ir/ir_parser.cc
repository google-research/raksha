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

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/float_attribute.h"
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

IrProgramParser::ConstructOperationResult IrProgramParser::ConstructOperation(
    IrParser::OperationContext& operation_context,
    BlockBuilder& block_builder) {
  // Operator
  std::string operator_text = operation_context.ID()->getText();
  if (!context_->IsRegisteredOperator(operator_text)) {
    context_->RegisterOperator(std::make_unique<Operator>(
        operator_text, operation_context.result()->VALUE_ID().size()));
  }
  // Grammar rule for attributes is either
  //   -> ID ':' FLOATLITERAL (a double-precision floating point number type) or
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
            {CHECK_NOTNULL(string_attribute_context)->ID()->getText(),
             Attribute::Create<StringAttribute>(
                 CHECK_NOTNULL(string_attribute_context)
                     ->stringLiteral()
                     ->getText())});
        continue;
      }
      if (auto* float_attribute_context =
              dynamic_cast<IrParser::FloatAttributeContext*>(
                  attribute_context)) {
        double parsed_float = 0;
        auto text = CHECK_NOTNULL(float_attribute_context)
                        ->FLOATLITERAL()
                        ->getText();
        // Discard the suffix (l or f).
        auto last_char_it = text.rbegin();
        if (last_char_it != text.rend() && *last_char_it == 'l') {
          text = std::string(text.begin(), text.end() - 1);
        }
        bool conversion_succeeds = absl::SimpleAtod(text, &parsed_float);
        CHECK(conversion_succeeds == true);
        attributes.insert(
            {CHECK_NOTNULL(float_attribute_context)->ID()->getText(),
             Attribute::Create<FloatAttribute>(parsed_float)});
        continue;
      }
      auto* num_attribute_context =
          dynamic_cast<IrParser::NumAttributeContext*>(attribute_context);
      int64_t parsed_int = 0;
      bool conversion_succeeds = absl::SimpleAtoi(
          CHECK_NOTNULL(num_attribute_context)->NUMLITERAL()->getText(),
          &parsed_int);
      CHECK(conversion_succeeds == true);
      attributes.insert(
          {CHECK_NOTNULL(num_attribute_context)->ID()->getText(),
           Attribute::Create<Int64Attribute>(parsed_int)});
    }
  }

  // value_names stores the string of inputs
  std::vector<std::string> value_names;
  if (operation_context.argumentList() != nullptr) {
    for (IrParser::ValueContext* value_context :
         operation_context.argumentList()->value()) {
      if (dynamic_cast<IrParser::AnyValueContext*>(value_context)) {
        value_names.push_back("<<ANY>>");
        continue;
      }
      auto* named_value_context =
          dynamic_cast<IrParser::NamedValueContext*>(value_context);
      // Operation Result Value
      value_names.push_back(
          CHECK_NOTNULL(named_value_context)->VALUE_ID()->getText());
    }
  }

  auto op = std::make_unique<Operation>(
      nullptr,
      *CHECK_NOTNULL(
          context_->GetOperator(operation_context.ID()->getText())),
      std::move(attributes), ValueList(), nullptr);

  auto op_names =
      utils::MapIter<std::string>(operation_context.result()->VALUE_ID(),
                                  [](auto arg) { return arg->getText(); });
  return IrProgramParser::ConstructOperationResult{
      .op_return_value_names = std::move(op_names),
      .operation = std::move(op),
      .input_names = std::move(value_names)};
}
void IrProgramParser::ConstructBlock(IrParser::BlockContext& block_context) {
  BlockBuilder builder;
  absl::flat_hash_map<std::string, Value> value_map;
  std::vector<ConstructOperationResult> construct_operation_results;

  for (IrParser::OperationContext* operation_context :
       block_context.operation()) {
    auto construct_operation_result = IrProgramParser::ConstructOperation(
        *CHECK_NOTNULL(operation_context), builder);
    // operationResult value name is expected to be "out.<index number in the
    // result list>""
    uint64_t index = 0;
    for (const auto& op_name :
         construct_operation_result.op_return_value_names) {
      const Value& v = Value::MakeOperationResultValue(
          *construct_operation_result.operation, absl::StrCat(".", index));
      index++;
      value_map.insert({op_name, v});
      ssa_names_->AddID(v, op_name);
    }
    construct_operation_results.push_back(
        std::move(construct_operation_result));
  }

  for (auto& [op_names, op, input_names] : construct_operation_results) {
    for (const auto& op_value : input_names) {
      if (op_value == "<<ANY>>") {
        op->AddInput(Value(value::Any()));
        continue;
      }
      auto find_value = value_map.find(op_value);
      CHECK(find_value != value_map.end()) << "Value not found " << op_value;
      op->AddInput(find_value->second);
    }
    builder.AddOperation(std::move(op));
  }
  const Block& b = module_->AddBlock(builder.build());
  block_map_.insert({block_context.ID()->getText(), std::addressof(b)});
  ssa_names_->AddID(b, block_context.ID()->getText());
}

void IrProgramParser::ConstructModule(IrParser::ModuleContext& module_context) {
  for (IrParser::BlockContext* block_context : module_context.block()) {
    IrProgramParser::ConstructBlock(*CHECK_NOTNULL(block_context));
  }
  ssa_names_->AddID(*module_, module_context.ID()->getText());
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
