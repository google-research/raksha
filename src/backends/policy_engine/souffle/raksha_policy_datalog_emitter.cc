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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/statusor.h"
#include "src/backends/policy_engine/souffle/utils.h"
#include "src/ir/auth_logic/ast_construction.h"
#include "src/ir/auth_logic/lowering_ast_datalog.h"
#include "src/ir/auth_logic/souffle_emitter.h"
#include "src/ir/auth_logic/universe_relation_insertion.h"

ABSL_FLAG(std::string, policy_rules, "", "file containing policy rules");
ABSL_FLAG(std::string, datalog_file, "", "path to policy directory");

constexpr char kUsageMessage[] =
    "This tool takes a file containing policy, converts it to datalog and "
    "temporary stores it in all_policy_verifier_interface.dl";

namespace {

absl::StatusOr<std::string> ReadFileContents(std::filesystem::path file_path) {
  if (!std::filesystem::exists(file_path)) {
    return absl::NotFoundError(
        absl::StrCat("File '", file_path.string(), "' does not exist!"));
  }

  std::ifstream file_stream(file_path);
  if (!file_stream) {
    return absl::FailedPreconditionError(absl::StrCat(
        "Unable to read file '", file_path.string(), "': ", strerror(errno)));
  }

  // Read the entire file as ostringstream and convert to std::string.
  std::ostringstream string_stream;
  string_stream << file_stream.rdbuf();
  return string_stream.str();
}

}  // namespace

using AuthLogicProgram = raksha::ir::auth_logic::Program;
using DatalogProgram = raksha::ir::datalog::Program;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging("check_policy_compliance");
  absl::SetProgramUsageMessage(kUsageMessage);
  absl::ParseCommandLine(argc, argv);

  // Read the sql policy rules file.
  absl::StatusOr<std::string> policy_rules =
      ReadFileContents(absl::GetFlag(FLAGS_policy_rules));
  if (!policy_rules.ok()) {
    LOG(ERROR) << "Error reading policy rules file: " << policy_rules.status();
    return 2;
  }

  std::filesystem::path datalog_file_path(absl::GetFlag(FLAGS_datalog_file));

  // Invoke the parser for auth_logic and return datalog
  AuthLogicProgram program =
      raksha::ir::auth_logic::ParseProgram(policy_rules.value());
  program = raksha::ir::auth_logic::InsertUniverseRelations(program);
  DatalogProgram datalog_program =
      raksha::ir::auth_logic::LoweringToDatalogPass::Lower(program);
  std::string datalog_policy =
      raksha::ir::auth_logic::SouffleEmitter::EmitProgram(datalog_program);

  std::ofstream datalog_file(
      datalog_file_path, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!datalog_file) {
    LOG(ERROR) << "Error creating " << datalog_file_path << " :"
               << strerror(errno);
    std::cout << "Error creating file" << std::endl;
    return 1;
  }
  datalog_file << datalog_policy;

  return 0;
}
