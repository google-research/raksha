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

#include "src/xform_to_datalog/authorization_logic.h"

namespace raksha::xform_to_datalog {

std::optional<AuthorizationLogicDatalogFacts>
AuthorizationLogicDatalogFacts::create(
  const std::string& program_dir, const std::string& program
) {
  auto output_dir = std::filesystem::temp_directory_path();
  int res = generate_datalog_facts_from_authorization_logic(
    program.c_str(), program_dir.c_str(), output_dir.c_str());
  if (res) return std::nullopt;

  // Read the file into a string.
  std::filesystem::path output = output_dir / (program + ".dl");
  std::ifstream file_stream(output);
  if (!file_stream) return std::nullopt;

  std::string datalog_program;
  file_stream.seekg(0, std::ios::end);
  datalog_program.resize(file_stream.tellg());
  file_stream.seekg(0, std::ios::beg);
  file_stream.read(&datalog_program[0], datalog_program.size());

  return AuthorizationLogicDatalogFacts(datalog_program);
}

}  // namespace raksha::xform_to_datalog
