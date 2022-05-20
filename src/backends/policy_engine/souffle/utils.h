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

#ifndef SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_UTILS_H_
#define SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_UTILS_H_

#include <filesystem>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"

namespace raksha::backends::policy_engine::souffle {

absl::Status WriteFactsStringToFactsFile(
    const std::filesystem::path &facts_directory_path,
    absl::string_view relation_name, absl::string_view facts_string);

}  // namespace raksha::backends::policy_engine::souffle

#endif  // SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_UTILS_H_
