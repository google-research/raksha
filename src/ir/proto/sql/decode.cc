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

#include "src/ir/proto/sql/decode.h"
#include "src/ir/proto/sql/decoder_context.h"

namespace raksha::ir::proto::sql {

Value DecodeSourceTableColumn(
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
  return Value(value::StoredValue(
      decoder_context.GetOrCreateStorage(column_path)));
}

Value DecodeLiteral(
    const Literal &literal, DecoderContext &decoder_context) {
  const absl::string_view literal_str = literal.literal_str();
  CHECK(!literal_str.empty()) << "required field literal_str was empty.";
  return Value(value::OperationResult(
      decoder_context.MakeLiteralOperation(literal_str), "out"));
}

}  // namespace raksha::ir::proto::sql
