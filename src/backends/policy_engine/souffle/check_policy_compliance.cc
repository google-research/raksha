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
#include "src/ir/proto_to_ir.h"
#include "src/parser/ir/ir_parser.h"

ABSL_FLAG(std::optional<std::string>, ir, std::nullopt, "the IR file");
ABSL_FLAG(std::optional<std::string>, sql_policy_rules, std::nullopt,
          "file containing the SQL policy rules");
ABSL_FLAG(std::optional<std::string>, proto, std::nullopt, "the proto file");
ABSL_FLAG(std::optional<std::string>, policy_engine, std::nullopt,
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

enum class ReturnCode : uint8_t { PASS = 0, FAIL = 1 , ERROR = 2};

int UnwrapExitCode(ReturnCode code) { return static_cast<int>(code); }

struct IrGraphComponents {
  std::unique_ptr<raksha::ir::Module> ir_module;
  std::unique_ptr<raksha::ir::SsaNames> ir_ssa_names;
  std::unique_ptr<raksha::ir::IRContext> ir_context;
};

absl::StatusOr<IrGraphComponents> GetIrGraphComponentsFromIrPath(
    absl::string_view ir_path) {
  raksha::ir::IrProgramParser ir_parser;
  absl::StatusOr<std::string> ir_string = ReadFileContents(ir_path);
  if (ir_string.ok()) {
    auto [context, module, ssa_names] = ir_parser.ParseProgram(*ir_string);
    return IrGraphComponents{.ir_module = std::move(module),
                             .ir_ssa_names = std::move(ssa_names),
                             .ir_context = std::move(context)};
  } else {
    return ir_string.status();
  }
}

absl::StatusOr<IrGraphComponents> GetIrGraphComponentsFromProtoPath(
    absl::string_view proto_path) {
  absl::StatusOr<std::string> proto_string = ReadFileContents(proto_path);
  if (proto_string.ok()) {
    std::unique_ptr<raksha::ir::IRContext> context =
        std::make_unique<raksha::ir::IRContext>(); /*...*/
    absl::StatusOr<raksha::ir::ProtoToIR::Result> ir_result =
        raksha::ir::ProtoToIR::Convert(*context, *proto_string);
    if (ir_result.ok()) {
      return IrGraphComponents{
          .ir_module = std::move(ir_result.value().module),
          .ir_ssa_names = std::move(ir_result.value().ssa_names),
          .ir_context = std::move(context)};
    } else {
      return ir_result.status();
    }
  } else {
    LOG(ERROR) << "Error reading proto file: " << proto_string.status();
    return proto_string.status();
  }
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

  // Read the sql policy rules file.
  absl::StatusOr<std::string> sql_policy_rules =
      ReadFileContents(absl::GetFlag(FLAGS_sql_policy_rules).value());
  if (absl::GetFlag(FLAGS_sql_policy_rules).has_value() && !sql_policy_rules.ok()) {
    LOG(ERROR) << "Error reading sql policy rules file: "
               << sql_policy_rules.status();
    return UnwrapExitCode(ReturnCode::ERROR);
  }

  const std::optional<std::string> ir_path = absl::GetFlag(FLAGS_ir);
  const std::optional<std::string> proto_path = absl::GetFlag(FLAGS_proto);

  if (ir_path.has_value() == proto_path.has_value()) {
    if (ir_path.has_value()) {
      LOG(ERROR) << "Both --ir and --proto cannot be used simultaneously.";
    } else {
      LOG(ERROR) << "One of --ir or --proto must be specified.";
    }

    return UnwrapExitCode(ReturnCode::ERROR);
  }

  // Parse either an IR file or a Proto file
  const absl::StatusOr<IrGraphComponents> components =
      ir_path.has_value() ? GetIrGraphComponentsFromIrPath(*ir_path)
                          : GetIrGraphComponentsFromProtoPath(*proto_path);
  if (!components.ok()) {
    LOG(ERROR) << "Error during parsing graph " << components.status();
    return UnwrapExitCode(ReturnCode::ERROR);
  }

  // Invoke policy checker and return result.
  bool policyCheckSucceeded = false;
  if (absl::GetFlag(FLAGS_policy_engine).has_value()) {
    AuthLogicPolicy policy(absl::GetFlag(FLAGS_policy_engine).value());
    policyCheckSucceeded =
        SoufflePolicyChecker().IsModulePolicyCompliant(*components.value().ir_module, policy);
  } else if (absl::GetFlag(FLAGS_sql_policy_rules).has_value()) {
    SqlPolicyRulePolicy policy(*sql_policy_rules);
    policyCheckSucceeded =
        SoufflePolicyChecker().IsModulePolicyCompliant(*components.value().ir_module, policy);
  } else {
    LOG(ERROR) << "Both sql_policy_rules and authorzation logic generated "
                  "datalog policy engine are undefined";
    return 2;
  }

  if (policyCheckSucceeded) {
    LOG(ERROR) << "Policy check succeeded!";
    return UnwrapExitCode(ReturnCode::PASS);
  } else {
    LOG(ERROR) << "Policy check failed!";
    return UnwrapExitCode(ReturnCode::FAIL);
  }
}
