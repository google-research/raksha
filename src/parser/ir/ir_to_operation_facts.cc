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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "src/backends/policy_engine/souffle/datalog_lowering_visitor.h"
#include "src/common/logging/logging.h"
#include "src/parser/ir/ir_parser.h"

ABSL_FLAG(std::string, ir_file, "", "The file containing the IR program.");
ABSL_FLAG(std::string, out, "",
          "The file to which we should write the resulting isOperation facts.");

namespace raksha::parser::ir {
namespace {

using ::raksha::backends::policy_engine::souffle::DatalogLoweringVisitor;
using ::raksha::ir::IrProgramParser;

constexpr char kUsageMessage[] =
    "This tool takes a Raksha IR file and generates an `isOperation.facts` "
    "file for Souffle.";

int MainInner(int argc, char** argv) {
  /*google::InitGoogleLogging("generate_datalog_program");*/
  absl::SetProgramUsageMessage(kUsageMessage);
  absl::ParseCommandLine(argc, argv);

  std::filesystem::path ir_path =
      std::filesystem::path(absl::GetFlag(FLAGS_ir_file));
  if (!std::filesystem::exists(ir_path)) {
    LOG(ERROR) << "IR input file " << ir_path << " does not exist!";
    return 1;
  }

  std::ifstream ir_stream(ir_path);
  if (!ir_stream) {
    LOG(ERROR) << "Error reading IR file " << ir_path << ":" << strerror(errno);
    return 1;
  }

  std::ostringstream string_stream;
  string_stream << ir_stream.rdbuf();
  IrProgramParser parser;

  IrProgramParser::Result result = parser.ParseProgram(string_stream.str());
  DatalogLoweringVisitor datalog_lowering_visitor(std::move(*result.ssa_names));
  result.module->Accept(datalog_lowering_visitor);

  std::filesystem::path out_path =
      std::filesystem::path(absl::GetFlag(FLAGS_out));
  std::ofstream out_stream(out_path,
                           std::ios::out | std::ios::trunc | std::ios::binary);
  if (!out_stream) {
    LOG(ERROR) << "Error creating " << out_path << ":" << strerror(errno);
    return 1;
  }
  out_stream
      << absl::StrJoin(
             datalog_lowering_visitor.datalog_facts().is_operation_facts(),
             "\n",
             [](std::string* out, const auto& arg) {
               absl::StrAppend(out, arg.ToDatalogFactsFileString());
             })
      << "\n";
  return 0;
}

}  // namespace
}  // namespace raksha::parser::ir

int main(int argc, char** argv) {
  return raksha::parser::ir::MainInner(argc, argv);
}
