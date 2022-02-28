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

ir::Value DecodeSourceTableColumn(
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
  return ir::Value{value::StoredValue(
      decoder_context.GetOrCreateStorage(column_path))};
}

ir::Value DecodeLiteral(
    const Literal &literal, DecoderContext &decoder_context) {
  const std::string &literal_str = literal.literal_str();
  CHECK(!literal_str.empty()) << "required field literal_str was empty.";

  // For now, we are abusing the `Storage` IR node to encode literals. This is
  // not as crazy as it may initially look: just like a `Storage`, a `Literal`
  // is a global name that should always return the same value if no
  // modifications have been made to it. The only difference between a true
  // `Storage` and a `Literal` is that it is illegal to modify a `Literal`.
  // This difference is even less important for the SQL verifier we are
  // currently trying to integrate with, as the SQL verifier considers a
  // single query upon a table at a time. While this single query is running,
  // the transactionality guarantees of the database should force the view onto
  // the table to be constant through the life of that query, making the
  // columns we are also capturing in `Storage`s constant anyway.
  //
  // Prefix the literal string with "literal:" to reduce the chance of a
  // collision.
  return ir::Value{value::StoredValue(decoder_context.GetOrCreateStorage(
      absl::StrCat("literal:", literal_str)))};
}

}  // namespace raksha::ir::proto::sql
