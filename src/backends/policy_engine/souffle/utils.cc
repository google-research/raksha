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

#include "src/backends/policy_engine/souffle/utils.h"

#include <fstream>

#include "absl/strings/str_format.h"

namespace raksha::backends::policy_engine::souffle {

absl::Status WriteFactsStringToFactsFile(
    const std::filesystem::path &facts_directory_path,
    std::string_view relation_name, std::string_view facts_string) {
  std::filesystem::path fact_files_path(
      facts_directory_path /
      std::string(absl::StrFormat("%s.facts", relation_name)));
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

  facts_file << facts_string;
  return absl::OkStatus();
}

}  // namespace raksha::backends::policy_engine::souffle
