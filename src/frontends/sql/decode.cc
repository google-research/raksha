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
#include "src/ir/value.h"

namespace raksha::frontends::sql {

using ir::Operation;
using ir::Value;
using ir::value::StoredValue;

static Value WrapOperationResultValue(const Operation &operation) {
  return DecoderContext::WrapOperationResultValue(operation);
}

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
  return WrapOperationResultValue(
      decoder_context.MakeLiteralOperation(literal_str));
}

static Value DecodeMergeOperation(const MergeOperation &merge_operation,
                                  DecoderContext &decoder_context) {
  // Although the proto separates the inputs and the control inputs, we can't
  // really take advantage of that yet. We do, however, expect that at least one
  // regular input is provided.
  CHECK(merge_operation.inputs_size() >= 1)
      << "Each MergeOperation is expected to have at least one non-control "
         "input.";
  const auto &inputs = merge_operation.inputs();
  const auto &control_inputs = merge_operation.control_inputs();
  return WrapOperationResultValue(decoder_context.MakeMergeOperation(
      std::vector<uint64_t>(inputs.begin(), inputs.end()),
      std::vector<uint64_t>(control_inputs.begin(), control_inputs.end())));
}

static Value DecodeTagTransform(const TagTransform &tag_transform,
                                DecoderContext &decoder_context) {
  uint64_t transformed_node_id = tag_transform.transformed_node();
  CHECK(transformed_node_id != 0)
      << "Required TagTransform field transformed_node not present.";
  std::string transform_rule_name = tag_transform.transform_rule_name();
  CHECK(!transform_rule_name.empty())
      << "Required TagTransform field transform_rule_name not present.";
  absl::flat_hash_map<std::string, uint64_t> preconditions(
      tag_transform.tag_preconditions().begin(),
      tag_transform.tag_preconditions().end());
  return WrapOperationResultValue(decoder_context.MakeTagTransformOperation(
      transformed_node_id, transform_rule_name, preconditions));
}

static Value DecodeExpression(const Expression &expr,
                              DecoderContext &decoder_context) {
  // TODO(#413): Figure out what to do with the optional name field.
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
      return DecodeTagTransform(expr.tag_transform(), decoder_context);
    }
  }
  // Placate the compiler.
  CHECK(false) << "Unreachable!";
}

// Decode every `Expression` contained within the arena and return the top
// level `ir::Value` for that arena.
Value DecodeExpressionArena(
    const ExpressionArena &expr_arena, DecoderContext &decoder_context) {
  std::optional<Value> top_level_value = std::nullopt;
  for (const IdExpressionPair &id_expr_pair :
       expr_arena.id_expression_pairs()) {
    Value value = DecodeExpression(id_expr_pair.expression(), decoder_context);
    decoder_context.RegisterValue(id_expr_pair.id(), value);
    top_level_value = value;
  }
  CHECK(top_level_value.has_value()) << "Expected at least one expression to "
                                        "be provided in the ExpressionArena.";
  return *top_level_value;
}

Value DecodeExpressionArena(const ExpressionArena &expr_arena) {
  ir::IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  return DecodeExpressionArena(expr_arena, decoder_context);
}


}  // namespace raksha::frontends::sql
