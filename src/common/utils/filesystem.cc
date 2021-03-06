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

#include "src/common/utils/filesystem.h"

#include <cstdio>
#include <errno.h>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/common/logging/logging.h"

namespace raksha::common::utils {

absl::StatusOr<std::filesystem::path> CreateTemporaryDirectory() {
  std::error_code error_code;
  std::filesystem::path temp_path =
      std::filesystem::temp_directory_path(error_code);
  if (error_code) {
    return absl::FailedPreconditionError(absl::StrFormat(
        "Unable to access temporary file directory: %s", error_code.message()));
  }

  std::string target_dir = absl::StrFormat("%s/XXXXXX", temp_path);
  if (::mkdtemp(target_dir.data()) == nullptr) {
    return absl::FailedPreconditionError(
        absl::StrFormat("Unable to create target directory `%s`: %s", target_dir,
                        strerror(errno)));
  }

  return std::filesystem::path(target_dir);
}

}  // namespace raksha::common::utils
