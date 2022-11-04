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

#include "src/parser/sql/sql_parser.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "src/common/containers/hash_map.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "zetasql/public/analyzer.h"
#include "zetasql/public/parse_helpers.h"

namespace raksha::parser::sql {

absl::Status ParseStatement(std::string_view sql) {
  return zetasql::IsValidStatementSyntax(sql,
                                         zetasql::ERROR_MESSAGE_WITH_PAYLOAD);
}

}  // namespace raksha::parser::sql
