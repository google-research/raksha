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
//
// Tool that takes a manifest proto and generates corresponding datalog facts.

#include <filesystem>
#include <fstream>
#include <iostream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "src/common/logging/logging.h"
#include "src/ir/datalog_print_context.h"
#include "src/ir/proto/system_spec.h"
#include "src/ir/system_spec.h"
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"
#include "src/xform_to_datalog/datalog_facts.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"

ABSL_FLAG(std::string, datalog_file, "", "output file for the datalog facts");
ABSL_FLAG(std::string, manifest_proto, "", "The manifest proto file.");
ABSL_FLAG(std::string, auth_logic_file, "",
          "The file with authorization logic facts.");
ABSL_FLAG(bool, overwrite, false,
          "Should we overwrite the output file if it exists.");

constexpr char kUsageMessage[] =
    "This tool takes a manifest proto and generates a datalog program.";

using ManifestDatalogFacts = raksha::xform_to_datalog::ManifestDatalogFacts;
using AuthorizationLogicDatalogFacts =
    raksha::xform_to_datalog::AuthorizationLogicDatalogFacts;

int main(int argc, char *argv[]) {
  google::InitGoogleLogging("generate_datalog_program");
  absl::SetProgramUsageMessage(kUsageMessage);
  absl::ParseCommandLine(argc, argv);

  // Verify command line arguments
  std::filesystem::path manifest_filepath(absl::GetFlag(FLAGS_manifest_proto));
  if (!std::filesystem::exists(manifest_filepath)) {
    LOG(ERROR) << "Manifest proto file " << manifest_filepath
               << " does not exist!";
    return 1;
  }

  std::filesystem::path auth_logic_filepath(
      absl::GetFlag(FLAGS_auth_logic_file));
  if (!std::filesystem::exists(auth_logic_filepath)) {
    LOG(ERROR) << "Authorization logic file " << auth_logic_filepath
               << " does not exist!";
    return 1;
  }

  bool force_overwrite = absl::GetFlag(FLAGS_overwrite);
  std::filesystem::path datalog_filepath(absl::GetFlag(FLAGS_datalog_file));
  if (!force_overwrite && std::filesystem::exists(datalog_filepath)) {
    LOG(ERROR) << "Output datalog file " << datalog_filepath
               << " exists! Forgot `--overwrite`?";
    return 1;
  }

  // Parse manifest stream and dump as datalog program.
  std::ifstream manifest_proto_stream(manifest_filepath);
  if (!manifest_proto_stream) {
    LOG(ERROR) << "Error reading manifest proto file " << manifest_filepath
               << ":" << strerror(errno);
    return 1;
  }

  arcs::ManifestProto manifest_proto;
  if (!manifest_proto.ParseFromIstream(&manifest_proto_stream)) {
    LOG(ERROR) << "Error parsing the manifest proto " << manifest_filepath;
  }

  // Turn each ParticleSpecProto indicated in the manifest_proto into a
  // ParticleSpec object, which we can use directly.
  std::unique_ptr<raksha::ir::SystemSpec> system_spec =
      raksha::ir::proto::Decode(manifest_proto);
  CHECK(system_spec != nullptr);
  auto manifest_datalog_facts = ManifestDatalogFacts::CreateFromManifestProto(
      *system_spec, manifest_proto);

  std::filesystem::path auth_logic_filename = auth_logic_filepath.filename();
  auth_logic_filepath.remove_filename();
  std::optional<AuthorizationLogicDatalogFacts> auth_logic_datalog_facts =
      AuthorizationLogicDatalogFacts::create(auth_logic_filepath.c_str(),
                                             auth_logic_filename.c_str());

  if (!auth_logic_datalog_facts.has_value()) {
    LOG(ERROR) << "Unable to parse authorization logic file.\n";
    return 1;
  }

  auto datalog_facts = raksha::xform_to_datalog::DatalogFacts(
      manifest_datalog_facts, *auth_logic_datalog_facts);

  std::ofstream datalog_file(datalog_filepath, std::ios::out | std::ios::trunc |
                                                   std::ios::binary);
  if (!datalog_file) {
    LOG(ERROR) << "Error creating " << datalog_filepath << " :"
               << strerror(errno);
    return 1;
  }

  raksha::ir::DatalogPrintContext ctxt;
  datalog_file << datalog_facts.ToDatalog(ctxt);

  return 0;
}
