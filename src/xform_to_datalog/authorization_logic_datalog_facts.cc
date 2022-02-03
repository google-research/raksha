//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/xform_to_datalog/authorization_logic.h"
#include "src/common/logging/logging.h"

namespace raksha::xform_to_datalog {

std::optional<AuthorizationLogicDatalogFacts>
AuthorizationLogicDatalogFacts::create(
  const std::filesystem::path &program_dir, absl::string_view program) {
  const std::filesystem::path result_dir =
    std::filesystem::temp_directory_path();

  // List of relations to not declare in the generated auth logic code
  // because we already have definitions within the Raksha dataflow files for
  // this purpose. We use an vector here and create the composite string to pass
  // into the Rust code with StrJoin to make the code a bit more readable and to
  // prevent typos changing the interpretation of the list.
  const std::vector<absl::string_view> kRelationsToNotDeclare{
      "says_ownsTag",
      "says_ownsAccessPath",
      "says_hasTag",
      "says_canSay_hasTag",
      "says_removeTag",
      "says_canSay_removeTag",
      "says_may",
      "says_will",
      "isAccessPath",
      "isTag",
      "isPrincipal"
  };

  int res = GenerateDatalogFactsFromAuthorizationLogic(
    program, program_dir, result_dir, kRelationsToNotDeclare);
  if (res) {
    LOG(ERROR) << "Failure running the authorization logic compiler.\n";
    return std::nullopt;
  }
  // Read the file into a string.
  std::filesystem::path result = result_dir / (absl::StrCat(program, ".dl"));
  std::ifstream file_stream(result);
  if (!file_stream) {
    LOG(ERROR) << "Unable to read result of authorization logic compiler.\n";
    return std::nullopt;
  }

  // Determine file size.
  file_stream.seekg(0, std::ios::end);
  std::ifstream::pos_type filesize = file_stream.tellg();
  if (filesize == -1) {
    LOG(ERROR) << "Unable to determine the size of the result file.\n";
    return std::nullopt;
  }
  // Initialize and set size of buffer to avoid reallocations.
  std::string datalog_program(filesize, '\0');

  // Read the contents of the file into string buffer.
  file_stream.seekg(0, std::ios::beg);
  file_stream.read(datalog_program.data(), datalog_program.size());
  CHECK(file_stream.gcount() == filesize)
    << "Failure reading bytes from the result file.\n";

  return AuthorizationLogicDatalogFacts(datalog_program);
}

}  // namespace raksha::xform_to_datalog
