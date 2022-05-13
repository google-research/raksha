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
#ifndef SRC_FRONTENDS_SQL_OPS_OP_TRAITS_H_
#define SRC_FRONTENDS_SQL_OPS_OP_TRAITS_H_

#include "absl/strings/string_view.h"

namespace raksha::frontends::sql {

template <typename Op>
struct OpTraits {};

#define DEFINE_OP_TRAITS(CLASS, OP_NAME)                \
  class CLASS;                                          \
  template <>                                           \
  struct OpTraits<CLASS> {                              \
    static constexpr absl::string_view kName = OP_NAME; \
  }

DEFINE_OP_TRAITS(LiteralOp, "sql.literal");
DEFINE_OP_TRAITS(MergeOp, "sql.merge");
DEFINE_OP_TRAITS(TagTransformOp, "sql.tag_transform");
DEFINE_OP_TRAITS(SqlOutputOp, "sql.sql_output");

#undef DEFINE_OP_TRAITS

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_OPS_OP_TRAITS_H_
