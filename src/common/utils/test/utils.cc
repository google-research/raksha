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
#include "src/common/utils/test/utils.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"

namespace raksha::utils::test {

std::filesystem::path GetTestDataDir(std::string path) {
  const char *test_srcdir_env_chars = std::getenv("TEST_SRCDIR");
  const char * test_workspace_env_chars = std::getenv("TEST_WORKSPACE");
  
  // Behavior is undefined if building from nullptr (until C++23 when it is
  // disallowed) so check for nullptr specially.
  CHECK(test_srcdir_env_chars != nullptr);
  CHECK(test_workspace_env_chars != nullptr);

  std::string_view test_srcdir_env(test_srcdir_env_chars);
  std::string_view test_workspace_env(test_workspace_env_chars);
  CHECK(!test_srcdir_env.empty());
  CHECK(!test_workspace_env.empty());
  return std::filesystem::path(test_srcdir_env) /
         std::filesystem::path(test_workspace_env) / std::move(path);
}

std::vector<std::string> ReadFileLines(const std::filesystem::path& file) {
  // Probably not quite efficient, but should serve the purpose for tests.
  std::ifstream file_stream(file);
  EXPECT_TRUE(file_stream) << "Unable to open file " << file;
  std::vector<std::string> result;
  for (std::string line; std::getline(file_stream, line);) {
    result.push_back(line);
  }
  return result;
}

}  // namespace raksha::utils::test
