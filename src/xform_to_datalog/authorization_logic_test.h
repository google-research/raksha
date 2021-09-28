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
#include "src/xform_to_datalog/authorization_logic.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include "src/common/testing/gtest.h"

namespace fs = std::filesystem;

namespace raksha::xform_to_datalog {

class AuthorizationLogicTest : public ::testing::Test {
public:
  static fs::path GetTestDataDir() {
    const char* test_srcdir_env = std::getenv("TEST_SRCDIR");
    const char* test_workspace_env = std::getenv("TEST_WORKSPACE");
    EXPECT_NE(test_srcdir_env,  nullptr);
    EXPECT_NE(test_workspace_env, nullptr);
    return fs::path(test_srcdir_env) / fs::path(test_workspace_env) /
      "src" / "xform_to_datalog" / "testdata";
  }

  static std::vector<std::string> ReadFileLines(const fs::path& file) {
    // Probably not quite efficient, but should serve the purpose for tests.
    std::ifstream file_stream(file);
    EXPECT_TRUE(file_stream) << "Unable to open file " << file;

    std::vector<std::string> result;
    for (std::string line; std::getline(file_stream, line);) {
      result.push_back(line);
    }    
    return result;
  }
};

}  // namespace raksha::xform_to_datalog


