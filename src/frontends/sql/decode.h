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

#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {

// Decodes the `ExpressionArena` protobuf and returns the value resulting from
// evaluating the top-level expression. The SQL verifier will want to ensure
// that no confidentiality tags exist upon this expression.
ir::Value DecodeExpressionArena(const ExpressionArena &expr,
                                DecoderContext &decoder_context);

// A simple no-context variant for decoding the expression arena which creates
// a fresh context and calls the above.
ir::Value DecodeExpressionArena(const ExpressionArena &expr);

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_DECODE_H_
