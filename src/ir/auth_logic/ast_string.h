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
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"
#include "src/common/utils/map_iter.h"

namespace raksha::ir::auth_logic {

using AstNodeVariantType =
    std::variant<datalog::Predicate, datalog::ArgumentType, datalog::Argument,
                 datalog::RelationDeclaration, Principal, Attribute, CanActAs,
                 BaseFact, Fact, ConditionalAssertion, Assertion, SaysAssertion,
                 Query, Program>;

std::string ToString(AstNodeVariantType Node);

}  // namespace raksha::ir::auth_logic
