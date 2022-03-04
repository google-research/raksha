//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/frontends/sql/decode.h"

#include "src/frontends/sql/decoder_context.h"

namespace raksha::frontends::sql {

using ir::Value;
using ir::value::OperationResult;
using ir::value::StoredValue;

static Value DecodeSourceTableColumn(
    const SourceTableColumn &source_table_column,
    DecoderContext &decoder_context) {
  const std::string &column_path = source_table_column.column_path();
  CHECK(!column_path.empty()) << "Required field column_path was empty.";
  // Note: In the future, we will probably want to make a `Storage` per table
  // and grab columns from that table using operators. For now, however,
  // making each column an independent storage works for the MVP and is much
  // simpler.
  //
  // Also, for now, we consider all storages to have primitive type. We will
  // probably want to change that when we start handling types in a
  // non-trivial fashion.
  return Value(StoredValue(decoder_context.GetOrCreateStorage(column_path)));
}

static Value DecodeLiteral(const Literal &literal,
                           DecoderContext &decoder_context) {
  const absl::string_view literal_str = literal.literal_str();
  CHECK(!literal_str.empty()) << "required field literal_str was empty.";
  return Value(
      OperationResult(decoder_context.MakeLiteralOperation(literal_str),
                      DecoderContext::kDefaultOutputName));
}

static Value DecodeMergeOperation(const MergeOperation &merge_operation,
                                  DecoderContext &decoder_context) {
  // Although the proto separates the inputs and the control inputs, we can't
  // really take advantage of that yet. We do, however, expect that at least one
  // regular input is provided.
  CHECK(merge_operation.inputs_size() >= 1)
      << "Each MergeOperation is expected to have at least one non-control "
         "input.";
  std::vector<Value> direct_input_values;
  direct_input_values.reserve(merge_operation.inputs_size());
  for (Expression input_expr : merge_operation.inputs()) {
    direct_input_values.push_back(
        DecodeExpression(input_expr, decoder_context));
  }

  std::vector<Value> control_input_values;
  control_input_values.reserve(merge_operation.control_inputs_size());
  for (Expression control_expr : merge_operation.control_inputs()) {
    control_input_values.push_back(
        DecodeExpression(control_expr, decoder_context));
  }

  return Value(OperationResult(
      decoder_context.MakeMergeOperation(std::move(direct_input_values),
                                         std::move(control_input_values)),
      DecoderContext::kDefaultOutputName));
}

// A helper function to decode the specific subclass of the Expression.
static Value GetExprValue(const Expression &expr,
                          DecoderContext &decoder_context) {
  switch (expr.expr_variant_case()) {
    case Expression::EXPR_VARIANT_NOT_SET: {
      CHECK(false) << "Required field expr_variant not set.";
    }
    case Expression::kSourceTableColumn: {
      return DecodeSourceTableColumn(expr.source_table_column(),
                                     decoder_context);
    }
    case Expression::kLiteral: {
      return DecodeLiteral(expr.literal(), decoder_context);
    }
    case Expression::kMergeOperation: {
      return DecodeMergeOperation(expr.merge_operation(), decoder_context);
    }
    case Expression::kTagTransform: {
      CHECK(false) << "Not yet implemented!";
    }
  }
  // Placate the compiler.
  CHECK(false) << "Unreachable!";
}

Value DecodeExpression(const Expression &expr,
                       DecoderContext &decoder_context) {
  uint64_t id = expr.id();
  CHECK(id != 0) << "Required field id was not present in Expression.";
  // TODO(#413): Figure out what to do with the optional name field.
  Value value = GetExprValue(expr, decoder_context);
  decoder_context.RegisterValue(id, value);
  return value;
}

}  // namespace raksha::frontends::sql