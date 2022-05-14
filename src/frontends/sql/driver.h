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

#ifndef SRC_FRONTENDS_SQL_DRIVER_H_
#define SRC_FRONTENDS_SQL_DRIVER_H_

#include <filesystem>
#include <optional>

#include "src/backends/policy_engine/souffle/datalog_fact_policy.h"
#include "src/frontends/sql/sql_ir.pb.h"

namespace raksha::frontends::sql {

// The driver for the SQL analysis, to be attached to a third-party parsing
// frontend.
bool verify(const ExpressionArena &arena,
            backends::policy_engine::souffle::DatalogFactPolicy policy);

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_DRIVER_H_
