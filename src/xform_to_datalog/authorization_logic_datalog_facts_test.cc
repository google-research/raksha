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
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include "absl/strings/str_split.h"
#include "src/common/testing/gtest.h"
#include "src/xform_to_datalog/authorization_logic_test_utils.h"

namespace fs = std::filesystem;

namespace raksha::xform_to_datalog {

class AuthorizationLogicDatalogFactsTest : public AuthorizationLogicTest {};

TEST_F(AuthorizationLogicDatalogFactsTest, InvokesRustToolAndGeneratesOutput) {
  for (const std::string &program :
       {"simple_auth_logic", "empty_auth_logic", "says_owns_tag"}) {
    fs::path test_data_dir = GetTestDataDir();
    auto auth_facts =
        AuthorizationLogicDatalogFacts::create(test_data_dir.c_str(), program);
    ASSERT_TRUE(auth_facts.has_value());

    std::vector<std::string> actual_datalog =
      absl::StrSplit(auth_facts->ToDatalog(), "\n", absl::SkipEmpty());
    std::vector<std::string> expected_datalog =
        ReadFileLines(test_data_dir / (program + ".dl"));

    // Need to compare individual lines as the output order is
    // non-deterministic.
    ASSERT_THAT(actual_datalog,
                testing::UnorderedElementsAreArray(expected_datalog));
  }
}

TEST_F(AuthorizationLogicDatalogFactsTest, InvalidFileReturnsNullOpt) {
  auto auth_facts = AuthorizationLogicDatalogFacts::create("blah", "blah");
  EXPECT_EQ(auth_facts, std::nullopt);
}

}  // namespace raksha::xform_to_datalog
