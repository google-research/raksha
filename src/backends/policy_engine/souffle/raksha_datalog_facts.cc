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

#include "src/backends/policy_engine/souffle/utils.h"
#include "src/common/logging/logging.h"
#include "src/ir/datalog/input_relation_fact.h"

namespace raksha::backends::policy_engine::souffle {

namespace {

template <typename F>
absl::Status WriteFactsToFile(const std::filesystem::path &facts_path,
                              std::string_view relation_name, const F &facts) {
  return WriteFactsStringToFactsFile(
      facts_path, relation_name,
      absl::StrJoin(facts, "\n", [](std::string *out, const auto &arg) {
        absl::StrAppend(out, arg.ToDatalogFactsFileString());
      }));
}

}  // namespace

absl::Status RakshaDatalogFacts::DumpFactsToDirectory(
    const std::filesystem::path &facts_path,
    const std::vector<std::string> &empty_relations) const {
  // Verify that the desired directory exists.
  std::error_code error_code;
  if (!std::filesystem::exists(facts_path)) {
    return absl::FailedPreconditionError(
        absl::StrFormat("Requested directory `%s` is not found!", facts_path));
  }

  auto status =
      WriteFactsToFile(facts_path, "isOperation", is_operation_facts_);
  if (!status.ok()) return status;
  for (std::string_view name : empty_relations) {
    status = WriteFactsStringToFactsFile(facts_path, name, "");
    if (!status.ok()) return status;
  }

  CHECK(status.ok());
  return status;
}

}  // namespace raksha::backends::policy_engine::souffle
