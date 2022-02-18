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
  const Storage &storage = decoder_context.GetOrCreateStorage(column_path);

  // The result is initially just the stored value. However, we may wrap it
  // with an arbitrary number of tag_claim operations to add any tags that
  // the SQL verifier policy indicates are initially present upon a column.
  ir::Value result{value::StoredValue(storage)};

  for (const std::string &confidentiality_tag :
       source_table_column.confidentiality_tags()) {
    result = decoder_context.ClaimTagOnValue(
        std::move(result), confidentiality_tag, /*is_integrity=*/false);
  }
  for (const std::string &integrity_tag :
       source_table_column.integrity_tags()) {
    result = decoder_context.ClaimTagOnValue(
        std::move(result), integrity_tag, /*is_integrity=*/true);
  }

  return result;
}

}  // namespace raksha::ir::proto::sql
