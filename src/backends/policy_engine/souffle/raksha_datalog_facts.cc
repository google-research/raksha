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
#include "src/backends/policy_engine/souffle/raksha_datalog_facts.h"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "src/ir/datalog/input_relation_fact.h"

namespace raksha::backends::policy_engine::souffle {

namespace {

// For use below.
class DummyInputRelationFact
    : public ir::datalog::InputRelationFact<ir::datalog::Symbol> {
 public:
  virtual ~DummyInputRelationFact() {}
  absl::string_view GetRelationName() const { return "dummy"; }
};

template <typename F>
absl::Status WriteFactsToFile(const std::filesystem::path &facts_path,
                              absl::string_view relation_name, const F &facts) {
  std::filesystem::path fact_files_path(
      facts_path / std::string(absl::StrFormat("%s.facts", relation_name)));
  if (std::filesystem::exists(fact_files_path)) {
    return absl::FailedPreconditionError(
        absl::StrFormat("Facts file `%s` already exists!", fact_files_path));
  }
  std::ofstream facts_file(fact_files_path,
                           std::ios::out | std::ios::trunc | std::ios::binary);
  if (!facts_file) {
    return absl::FailedPreconditionError(
        absl::StrFormat("Unable to create facts file for %s: `%s`",
                        relation_name, std::strerror(errno)));
  }

  facts_file << absl::StrJoin(
      facts, "\n", [](std::string *out, const auto &arg) {
        absl::StrAppend(out, arg.ToDatalogFactsFileString());
      });
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::filesystem::path> RakshaDatalogFacts::DumpFactsToDirectory(
    const std::vector<std::string> &empty_relations,
    const std::filesystem::path &directory) const {
  std::filesystem::path facts_path = directory;
  if (directory.empty()) {
    // Create a temporary directory for these facts.
    facts_path = std::tmpnam(nullptr);
    std::error_code error_code;
    if (!std::filesystem::create_directory(facts_path, error_code)) {
      return absl::FailedPreconditionError(
          absl::StrFormat("Unable to create directory `%s`: `%s`", facts_path,
                          error_code.message()));
    }
  } else {
    // Verify that the desired directory exists.
    std::error_code error_code;
    if (!std::filesystem::exists(facts_path)) {
      return absl::FailedPreconditionError(absl::StrFormat(
          "Requested directory `%s` is not found!", facts_path));
    }
  }

  auto status =
      WriteFactsToFile(facts_path, "isOperation", is_operation_facts_);
  if (!status.ok()) return status;
  for (absl::string_view name : empty_relations) {
    status = WriteFactsToFile(facts_path, name,
                              std::vector<DummyInputRelationFact>({}));
    if (!status.ok()) return status;
  }

  return facts_path;
}
}  // namespace raksha::backends::policy_engine::souffle
