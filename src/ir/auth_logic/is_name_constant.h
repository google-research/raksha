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

#ifndef SRC_IR_AUTH_LOGIC_IS_NAME_CONSTANT_H_
#define SRC_IR_AUTH_LOGIC_IS_NAME_CONSTANT_H_

#include "absl/strings/string_view.h"

namespace raksha::ir::auth_logic {

// Determine if an identifier (which may be an argument of a predicate
// or a principal name) is a constant or variable.
// TODO(#673 aferr) it would be better to refactor the AST so that
// identifiers are a separate AST node with constant and non-constant
// children determined at parse time rather than here since
// this information is available at parse time.
bool IsNameConstant(absl::string_view id);

};  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_IS_NAME_CONSTANT_H_
