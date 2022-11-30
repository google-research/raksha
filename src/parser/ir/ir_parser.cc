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

#include <algorithm>
#include <any>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeindex>
#include <utility>
#include <variant>
#include <vector>

#include "absl/log/check.h"
#include "src/common/utils/fold.h"
#include "src/common/utils/map_iter.h"
#include "src/common/utils/overloaded.h"
#include "src/common/utils/types.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/float_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrBaseVisitor.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrLexer.h"
#include "src/parser/ir/ir_parser_generator_grammar.inc/ir_parser_generator/IrParser.h"

namespace raksha::parser::ir {

using ir_parser_generator::IrBaseVisitor;
using ir_parser_generator::IrLexer;
using ir_parser_generator::IrParser;
using raksha::ir::Attribute;
using raksha::ir::BlockBuilder;
using raksha::ir::FloatAttribute;
using raksha::ir::Int64Attribute;
using raksha::ir::IRContext;
using raksha::ir::Module;
using raksha::ir::NamedAttributeMap;
using raksha::ir::Operation;
using raksha::ir::Operator;
using raksha::ir::SsaNames;
using raksha::ir::StringAttribute;
using raksha::ir::Value;
using raksha::ir::ValueList;

namespace {

using Any = std::any;

// During the parsing of the program, we will run into multiple kinds of value
// IDs, which cannot be fully constructed until we have finished the block
// processing but which we must keep track of. We use these `struct`s to keep
// track of those IDs.

// An ID indicating the "Any" value.
struct AnyId {};

// An ID indicating some SSA temporary.
struct TemporaryId {
  std::string name;
};

// A sum type that may store any legal ID.
using ValueId = std::variant<AnyId, TemporaryId>;

// If you pass a value into `any_cast` by value, it throws an exception if the
// cast is wrong. We would like to not use exceptions if we can help it, and we
// would like to not unnecessarily interrupt our dataflow with temporary
// variables, so we wrap calls to `any_cast` in this helper function that
// performs the creation of the temporary and the asserting-not-null for us.
template <class Result>
const Result& CheckAnyCast(const Any& input) {
  const Result* cast_input_ptr = std::any_cast<Result>(&input);
  CHECK(cast_input_ptr != nullptr);
  return *cast_input_ptr;
}

// An Antlr visitor that converts the IR parse tree into Raksha IR structures.
//
// Note: In many of this class's aggregation visitor functions, I use `auto` to
// capture the context name rather than the explicit name. This is to
// respect the visitor pattern's intentional ignorance of the exact nature
// of its children. This allows more easily switching out the child nodes
// under the attribute list in the grammar without disrupting the code here,
// so long as they produce the same result upon being visited.
class IrVisitor : public IrBaseVisitor {
 public:
  explicit IrVisitor()
      : result_{.context = std::make_unique<IRContext>(),
                .module = std::make_unique<Module>(),
                .ssa_names = std::make_unique<SsaNames>()} {}

  Any visitStringLiteral(
      IrParser::StringLiteralContext* string_context) override {
    std::string string_literal_contents_with_quotes =
        string_context->stringLiteralContents->getText();
    // Strip the quotes off of the end. This would be better to do in the lexer
    // but it's hard to get it to do that correctly.
    CHECK_GE(string_literal_contents_with_quotes.size(), 2);
    CHECK_EQ(string_literal_contents_with_quotes.front(), '\"');
    CHECK_EQ(string_literal_contents_with_quotes.back(), '\"');
    return string_context->stringLiteralContents->getText().substr(
        1, string_literal_contents_with_quotes.size() - 2);
  }

  Any visitAnyValue(IrParser::AnyValueContext* any_context) override {
    return ValueId(AnyId());
  }

  Any visitNamedTempValue(
      IrParser::NamedTempValueContext* named_temp_value_context) override {
    return ValueId(
        TemporaryId{.name = named_temp_value_context->valueName->getText()});
  }

  Any visitResult(IrParser::ResultContext* result_context) override {
    return utils::MapIter<ValueId>(
        result_context->namedTempValue(), [this](auto* child_context) {
          return CheckAnyCast<ValueId>(child_context->accept(this));
        });
  }

  Any visitArgumentList(
      IrParser::ArgumentListContext* argument_list_context) override {
    return utils::MapIter<ValueId>(
        argument_list_context->value(), [this](auto* child_context) {
          return CheckAnyCast<ValueId>(child_context->accept(this));
        });
  }

  Any visitFloatAttribute(
      IrParser::FloatAttributeContext* float_attribute_context) override {
    double parsed_float = 0;
    std::string text =
        ABSL_DIE_IF_NULL(float_attribute_context)->floatLiteral->getText();
    // Discard the suffix (l).
    CHECK(!text.empty());
    if (text.back() == 'l') {
      text = text.substr(0, text.size() - 1);
    }
    bool conversion_succeeds = absl::SimpleAtod(text, &parsed_float);
    CHECK(conversion_succeeds == true);
    return Attribute::Create<FloatAttribute>(parsed_float);
  }

  Any visitNumAttribute(
      IrParser::NumAttributeContext* num_attribute_context) override {
    int64_t parsed_int = 0;
    bool conversion_succeeds = absl::SimpleAtoi(
        ABSL_DIE_IF_NULL(num_attribute_context)->numLiteral->getText(),
        &parsed_int);
    CHECK(conversion_succeeds == true);
    return Attribute::Create<Int64Attribute>(parsed_int);
  }

  Any visitStringAttribute(
      IrParser::StringAttributeContext* string_attribute_context) override {
    return Attribute::Create<StringAttribute>(CheckAnyCast<std::string>(
        ABSL_DIE_IF_NULL(
            ABSL_DIE_IF_NULL(string_attribute_context)->stringLiteral())
            ->accept(this)));
  }

  Any visitAttribute(IrParser::AttributeContext* attribute_context) override {
    CHECK(attribute_context != nullptr);
    return std::make_pair(
        attribute_context->name->getText(),
        CheckAnyCast<Attribute>(
            attribute_context->attributePayload()->accept(this)));
  }

  Any visitAttributeList(
      IrParser::AttributeListContext* attribute_list_context) override {
    return common::utils::fold(
        attribute_list_context->attribute(), NamedAttributeMap(),
        [this](NamedAttributeMap attribute_map, auto* context) {
          auto name_attribute_pair =
              CheckAnyCast<std::pair<std::string, Attribute>>(
                  context->accept(this));
          auto insert_result = attribute_map.insert(name_attribute_pair);
          CHECK(insert_result.second) << "Multiple attributes with name: "
                                      << insert_result.first->first;
          return attribute_map;
        });
  }

  struct OperationParts {
    std::vector<ValueId> op_result_ids;
    std::string operator_name;
    NamedAttributeMap attributes;
    std::vector<ValueId> input_ids;
  };

  Any visitOperation(IrParser::OperationContext* operation_context) override {
    return OperationParts{
        .op_result_ids = CheckAnyCast<std::vector<ValueId>>(
            operation_context->result()->accept(this)),
        .operator_name = operation_context->operatorName->getText(),
        .attributes = VisitContextOrDefaultIfNull(
            operation_context->attributeList(), NamedAttributeMap()),
        .input_ids = VisitContextOrDefaultIfNull(
            operation_context->argumentList(), std::vector<ValueId>())};
  }

  Any visitBlock(IrParser::BlockContext* block_context) override {
    BlockBuilder block_builder;
    std::vector<std::pair<std::unique_ptr<Operation>, std::vector<ValueId>>>
        operations_and_inputs;
    absl::flat_hash_map<std::string, Value> temporary_name_to_value;

    // Due to potential cycles in the dataflow graph, we can't create operations
    // all at once. The values in the temporaries flowing into those operations
    // will likely be `OperationResult`s, which require a pointer to a built
    // operation, introducing a circular dependency. To break this dependency,
    // initially create operations without inputs, but associate the built
    // operations with the IDs of inputs they will eventually take. This allows
    // us to build up a map associating temporary names with the operations that
    // fill them, setting ourselves up to fill in those operation names later.
    operations_and_inputs.reserve(block_context->operation().size());
    for (auto* operation_context : block_context->operation()) {
      OperationParts operation_result =
          CheckAnyCast<OperationParts>(operation_context->accept(this));
      const Operator& op =
          MaybeRegisterOperator(operation_result.operator_name,
                                operation_result.op_result_ids.size());
      operations_and_inputs.push_back(
          {std::make_unique<Operation>(nullptr, op,
                                       std::move(operation_result.attributes),
                                       ValueList()),
           std::move(operation_result.input_ids)});
      const Operation& operation = *operations_and_inputs.back().first;
      for (uint64_t index = 0; index < operation_result.op_result_ids.size();
           ++index) {
        const ValueId& id = operation_result.op_result_ids.at(index);
        // AnyId should not be possible, as it is not an LValue.
        CHECK(!std::get_if<AnyId>(&id))
            << "Found an `Any` on the left hand side of an `Operation` "
               "assignment, which should not be possible!";
        auto insert_result = temporary_name_to_value.insert(
            {std::get<TemporaryId>(id).name,
             raksha::ir::Value::MakeOperationResultValue(operation, index)});
        CHECK(insert_result.second)
            << "Found temporary name which was assigned to twice: "
            << insert_result.first->first;
      }
    }

    // Now that we have collected all temporary values in one structure, add
    // all of them to `SsaNames`.
    for (const auto& [name, value] : temporary_name_to_value) {
      result_.ssa_names->AddID(value, name);
    }

    // Use our temporary name map to fill in the operation inputs and add them
    // to the block.
    for (auto& [operation, inputs] : operations_and_inputs) {
      ValueList input_list = utils::MapIter<Value>(
          inputs, [&temporary_name_to_value](const ValueId& id) {
            return std::visit(
                utils::overloaded{
                    [](AnyId any_id) {
                      return Value(raksha::ir::value::Any());
                    },
                    [&temporary_name_to_value](TemporaryId temporary_id) {
                      auto find_result =
                          temporary_name_to_value.find(temporary_id.name);
                      CHECK(find_result != temporary_name_to_value.end())
                          << "Value not found!";
                      return find_result->second;
                    }},
                id);
          });
      for (const Value& input : input_list) {
        operation->AddInput(input);
      }
      block_builder.AddOperation(std::move(operation));
    }
    result_.module->AddBlock(block_builder.build());
    return Unit();
  }

  Any visitModule(IrParser::ModuleContext* context) override {
    for (IrParser::BlockContext* block_context : context->block()) {
      ABSL_DIE_IF_NULL(block_context)->accept(this);
    }
    return Unit();
  }

  Any visitIrProgram(IrParser::IrProgramContext* context) override {
    context->module()->accept(this);
    return Unit();
  }

  IrProgramParserResult TakeResult() { return std::move(result_); }

 private:
  // A helper function that idempotently registers an operator and returns a
  // reference to it. Will fault if the operator has an inconsistent number of
  // return values.
  const Operator& MaybeRegisterOperator(absl::string_view operator_name,
                                        size_t num_results) {
    IRContext& context = *result_.context;
    if (!context.IsRegisteredOperator(operator_name)) {
      return context.RegisterOperator(
          std::make_unique<Operator>(operator_name, num_results));
    } else {
      const Operator& op =
          *ABSL_DIE_IF_NULL(context.GetOperator(operator_name));
      CHECK(op.number_of_return_values() == num_results)
          << "Operator has different number of return values in different "
             "instances "
          << operator_name;
      return op;
    }
  }

  template <class Result, class Context>
  Result VisitContextOrDefaultIfNull(Context* context, Result default_result) {
    return (context) ? CheckAnyCast<Result>(context->accept(this))
                     : default_result;
  }

  IrProgramParserResult result_;
};
}  // namespace

/// This function produces an abstract syntax tree (AST) rooted with a
/// program node when given the textual representation of a program.
IrProgramParserResult ParseProgram(absl::string_view prog_text) {
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
      *ABSL_DIE_IF_NULL(parser.irProgram());

  CHECK(parser.getNumberOfSyntaxErrors() == 0)
      << "Encountered syntax errors in IR parser, stopping!";

  IrVisitor ir_visitor;
  ir_visitor.visitIrProgram(&program_context);

  return ir_visitor.TakeResult();
}

}  // namespace raksha::parser::ir
