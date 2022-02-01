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
//-----------------------------------------------------------------------------

// This file encapsulates the dependency upon the external C-style linkage
// function generate_datalog_facts_from_authorization_logic, which in a normal
// build will be supplied by the Rust auth logic compiler. This allows us to
// not only prevent exposing a C-linkage declaration in a header, but it allows
// us to use conditional compilation to cut the link to the auth logic compiler
// with a #define local to a single translation unit.

#include "authorization_logic.h"

#include <filesystem>

#include "absl/strings/string_view.h"
#include "absl/strings/str_join.h"
#include "src/common/logging/logging.h"

// For now, we need to be able to disable authorization logic to smooth the
// process of importing Raksha into Google's internal codebase.
#ifndef DISABLE_AUTHORIZATION_LOGIC

// Generates datalog facts from the given program.
extern "C" int generate_datalog_facts_from_authorization_logic(
  const char* program, const char* program_path,
  const char* out_dir, const char* decl_skip_vec);

#endif

namespace raksha::xform_to_datalog {

int GenerateDatalogFactsFromAuthorizationLogic(
    absl::string_view program,
    const std::filesystem::path &program_dir,
    const std::filesystem::path &result_dir,
    const std::vector<absl::string_view> &relations_to_not_declare) {

  const std::string relations_to_not_declare_str =
    absl::StrJoin(relations_to_not_declare, ",");

  // Turn the macro DISABLE_AUTHORIZATION_LOGIC into an expression the compiler
  // understands so we can put it into the CHECK.
  constexpr bool auth_logic_enabled =
#ifdef DISABLE_AUTHORIZATION_LOGIC
    false
#else
    true
#endif
  ;

  CHECK(auth_logic_enabled)
    << "Attempted to use authorization logic compiler when it has been disabled"
   << " using DISABLE_AUTHORIZATION_LOGIC.";

  return 
#ifdef DISABLE_AUTHORIZATION_LOGIC
// If authorization logic has been disabled, return an exit code as if it
// always failed. This should be unreachable due to the CHECK, and acts only as
// a way to keep the compiler happy.
  1
#else
  generate_datalog_facts_from_authorization_logic(
      program.data(), program_dir.c_str(), result_dir.c_str(),
      relations_to_not_declare_str.data())
#endif
  ;
}

}
