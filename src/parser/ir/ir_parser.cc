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

#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
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

Operation IrProgramParser::ConstructOperation(
    IrParser::OperationContext& operation_context) {
  NamedAttributeMap attributes;

  for (IrParser::AttributeContext* attribute_context :
       operation_context.attributeList()->attribute()) {
    auto attribute_context_id_size =
        CHECK_NOTNULL(attribute_context)->ID().size();
    CHECK(attribute_context_id_size == 1 || attribute_context_id_size == 2);
    if (CHECK_NOTNULL(attribute_context)->ID().size() == 2) {
      attributes.insert(
          {CHECK_NOTNULL(attribute_context)->ID(0)->getText(),
           Attribute::Create<StringAttribute>(
               CHECK_NOTNULL(attribute_context)->ID(1)->getText())});
    } else {
      int64_t parsed_int = 0;
      bool conversion_succeeds = absl::SimpleAtoi(
          CHECK_NOTNULL(attribute_context)->NUMLITERAL()->getText(),
          &parsed_int);
      CHECK(conversion_succeeds == true);
      attributes.insert({CHECK_NOTNULL(attribute_context)->ID(0)->getText(),
                         Attribute::Create<Int64Attribute>(parsed_int)});
    }
  }

  ValueList inputs = utils::MapIter<Value>(
      operation_context.argumentList()->value(),
      [this](IrParser::ValueContext* value_context) {
        auto value_context_id_size = CHECK_NOTNULL(value_context)->ID().size();
        if (value_context_id_size == 0) {
          return Value(value::Any());
        }

        std::cout << value_context_id_size << std::endl;
        CHECK(value_context_id_size == 1 || value_context_id_size == 2);
        const std::string& value_id =
            CHECK_NOTNULL(value_context)->ID(0)->getText();
        auto find_value_id_operation = operation_map.find(value_id);
        if (find_value_id_operation != operation_map.end()) {
          return Value(value::OperationResult(find_value_id_operation->second,
                                              value_id));
        }
        types::TypeFactory type_factory;
        return Value(value::StoredValue(
            Storage(value_id, type_factory.MakePrimitiveType())));
      });
  if (context.GetOperator(operation_context.ID(1)->getText()) == nullptr) {
    context.RegisterOperator(
        std::make_unique<Operator>(operation_context.ID(1)->getText()));
  }
  return Operation(
      nullptr,
      *CHECK_NOTNULL(context.GetOperator(operation_context.ID(1)->getText())),
      std::move(attributes), std::move(inputs), nullptr);
}

/// This function produces an abstract syntax tree (AST) rooted with a
/// program node when given the textual representation of a program.
Operation IrProgramParser::ParseProgram(absl::string_view prog_text) {
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
  return IrProgramParser::ConstructOperation(
      *CHECK_NOTNULL(program_context.operation()));
}

}  // namespace raksha::ir
