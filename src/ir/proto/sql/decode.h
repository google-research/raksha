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

#ifndef SRC_IR_PROTO_SQL_MVP_DECODE_H_
#define SRC_IR_PROTO_SQL_MVP_DECODE_H_

#include <vector>

#include "src/ir/value.h"
#include "src/ir/ir_context.h"
#include "src/ir/proto/sql/sql_ir.pb.h"

namespace raksha::ir::proto::sql {

ir::Value DecodeSourceTableColumn(
    const SourceTableColumn &source_table_column,
    IRContext &ir_context);

}  // namespace raksha::ir::proto::sql

#endif  // SRC_IR_PROTO_SQL_MVP_DECODE_H_
