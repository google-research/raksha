/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_
#define SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_

/*
This file contains a translation that populates the universe relations.
Universe relations are facts of the form `isSymbol(x)` that are used to
keep datalog rules grounded and finite. This pattern works by putting facts
of the form `isSymbol(x)` on the RHS of a conditional rule where the variable
`x` would otherwise be ungrounded; then the set of symbols is populated by
adding unconditional facts of the form `isSymbol("specific_symbol").` to
populate these universes.

A problem with using this pattern in the context of authorization logic is that
these universes need to be shared across principals, and this is not
expressable in the sufrace language. Also, manually using this pattern is also
tedius, and makes the code less clear. This translation handles
both of these problems by:
    1) extending the program to implement the universe relation pattern by:
    adding universe predicates to the RHS of rules, and adding facts that add
    elements to universes.
    2) extending the program with facts that use delegation to pass membership
    of universes across principals.

This translation is a compiler pass that operates on the data structure
described in `ast.h` and produces a new instance of the data structure in
`ast.h`. It should come before the translation in `lowering_ast_datalog.h`.
*/

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"

#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_traversing_visitor.h"
#include "src/ir/auth_logic/declaration_environment.h"
#include "src/ir/auth_logic/type_environment.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

// Generate a new version of the program transformed to include
// the universe declarations, universe populating rules,
// and extensions of the rules to include conditions that check
// these universes.
Program InsertUniverseRelations(const Program& prog);

};  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_UNIVERSE_RELATION_INSERTION_H_
