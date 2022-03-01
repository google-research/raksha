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

#ifndef SRC_FRONTENDS_SQL_DECODE_H_
#define SRC_FRONTENDS_SQL_DECODE_H_

#include <vector>

#include "src/ir/value.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"

namespace raksha::frontends::sql {

const ir::Value &DecodeExpression(
    const Expression &expr, DecoderContext &decoder_context);

ir::Value DecodeSourceTableColumn(
    const SourceTableColumn &source_table_column,
    DecoderContext &decoder_context);

ir::Value DecodeLiteral(
    const Literal &literal, DecoderContext &decoder_context);

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_DECODE_H_
