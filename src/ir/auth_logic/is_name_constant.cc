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

#include "src/ir/auth_logic/is_name_constant.h"

#include <string_view>

#include "absl/strings/match.h"
#include "absl/strings/numbers.h"

namespace raksha::ir::auth_logic {


bool IsNameConstant(std::string_view id) {
  if (absl::StartsWith(id, "\"") && absl::EndsWith(id, "\"")) {
    return true;
  } else {
    int unused_output;
    // Numeric literals are constants and this is
    // a numeric literal if all the characters are numbers.
    return absl::SimpleAtoi(id, &unused_output);
  }
}

};  // namespace raksha::ir::auth_logic
