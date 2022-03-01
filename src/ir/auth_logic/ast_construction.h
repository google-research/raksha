
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

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.

#ifndef SRC_IR_AUTH_LOGIC_AST_CONSTRUCTION_H_
#define SRC_IR_AUTH_LOGIC_AST_CONSTRUCTION_H_

#include "src/ir/datalog/program.h"
#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

Program parse_program (const std::string& prog_text);

}// namespace raksha::ir::auth_logic

#endif

