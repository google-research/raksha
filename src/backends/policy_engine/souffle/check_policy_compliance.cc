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
#include "src/backends/policy_engine/auth_logic_policy.h"
#include "src/backends/policy_engine/policy.h"
#include "src/backends/policy_engine/souffle/souffle_policy_checker.h"
#include "src/backends/policy_engine/sql_policy_rule_policy.h"
#include "src/common/logging/logging.h"
#include "src/parser/ir/ir_parser.h"

ABSL_FLAG(std::string, ir, "", "the IR file");
ABSL_FLAG(std::optional<std::string>, sql_policy_rules, "",
          "file containing the SQL policy rules");
ABSL_FLAG(std::optional<std::string>, policy_engine, "",
          "name of the policy engine");

constexpr char kUsageMessage[] =
    "This tool takes an IR representation of a system, policy engine and "
    "returns whether it is policy compliant.";

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

using raksha::backends::policy_engine::AuthLogicPolicy;
using raksha::backends::policy_engine::SoufflePolicyChecker;
using raksha::backends::policy_engine::SqlPolicyRulePolicy;
using raksha::ir::IrProgramParser;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging("check_policy_compliance");
  absl::SetProgramUsageMessage(kUsageMessage);
  absl::ParseCommandLine(argc, argv);
  // Parse the given IR file.
  absl::StatusOr<std::string> ir_string =
      ReadFileContents(absl::GetFlag(FLAGS_ir));
  if (!ir_string.ok()) {
    LOG(ERROR) << "Error reading IR file: " << ir_string.status();
    return 2;
  }

  // Read the sql policy rules file.
  absl::StatusOr<std::string> sql_policy_rules =
      ReadFileContents(absl::GetFlag(FLAGS_sql_policy_rules).value());
  if (absl::GetFlag(FLAGS_sql_policy_rules).has_value() &&
      !sql_policy_rules.ok()) {
    LOG(ERROR) << "Error reading sql policy rules file: "
               << sql_policy_rules.status();
    return 2;
  }

  // Invoke policy checker and return result.
  IrProgramParser irParser;
  bool policyCheckSucceeded = false;
  auto [context, module, ssa_names] = irParser.ParseProgram(*ir_string);
  if (absl::GetFlag(FLAGS_policy_engine).has_value()) {
    AuthLogicPolicy policy(absl::GetFlag(FLAGS_policy_engine).value());
    policyCheckSucceeded =
        SoufflePolicyChecker().IsModulePolicyCompliant(*module, policy);
  } else if (absl::GetFlag(FLAGS_sql_policy_rules).has_value()) {
    SqlPolicyRulePolicy policy(*sql_policy_rules);
    policyCheckSucceeded =
        SoufflePolicyChecker().IsModulePolicyCompliant(*module, policy);
  } else {
    LOG(ERROR) << "Both sql_policy_rules and authorzation logic generated "
                  "datalog policy engine are undefined";
    return 2;
  }

  if (policyCheckSucceeded) {
    LOG(ERROR) << "Policy check succeeded!";
    return 0;
  } else {
    LOG(ERROR) << "Policy check failed!";
    return 1;
  }
}
