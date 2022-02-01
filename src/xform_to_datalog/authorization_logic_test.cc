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
#include "src/test_utils/utils.h"

namespace raksha::xform_to_datalog {

namespace fs = std::filesystem;

TEST(AuthorizationLogicTest, InvokesRustToolAndGeneratesOutput) {
  const fs::path& test_data_dir = test_utils::GetTestDataDir("src/xform_to_datalog/testdata");
  fs::path output_dir = fs::temp_directory_path();
  int res = GenerateDatalogFactsFromAuthorizationLogic(
    "simple_auth_logic", test_data_dir.c_str(), output_dir.c_str(), {});

  ASSERT_EQ(res, 0) << "Invoking authorization logic compiler failed.";

  std::vector<std::string> actual_datalog =
    test_utils::ReadFileLines(output_dir / "simple_auth_logic.dl");
  std::vector<std::string> expected_datalog =
    test_utils::ReadFileLines(test_data_dir / "simple_auth_logic.dl");

  // Need to compare individual lines as the output order is non-deterministic.
  ASSERT_THAT(actual_datalog,
	      testing::UnorderedElementsAreArray(expected_datalog));
}

TEST(AuthorizationLogicTest, ErrorsInRustToolReturnsNonZeroValue) {
  // Force the tool to return error by specifying non-existent files.
  int res = GenerateDatalogFactsFromAuthorizationLogic(
    "simple_auth_logic", "blah", "blah", {});
  ASSERT_EQ(res, 1);
}
  
}  // namespace raksha::xform_to_datalog
