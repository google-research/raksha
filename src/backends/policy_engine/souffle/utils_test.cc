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

#include <cstdio>
#include <fstream>
#include <sstream>

#include "absl/status/statusor.h"
#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/filesystem.h"

namespace raksha::backends::policy_engine::souffle {
namespace {

TEST(WriteFactsToBadDirectory, WriteFactsToBadDirectory) {
  std::filesystem::path bad_path("nosuchpath");
  ASSERT_FALSE(std::filesystem::exists(bad_path));

  absl::Status result = WriteFactsStringToFactsFile(bad_path, "SomeRelation", "");
  EXPECT_FALSE(result.ok());
  EXPECT_TRUE(IsFailedPrecondition(result));
}

class CreateTempdirTest : public testing::Test {
 protected:
  CreateTempdirTest() {
    absl::StatusOr<std::filesystem::path> tempdir_statusor =
        common::utils::CreateTemporaryDirectory();
    CHECK(tempdir_statusor.ok());
    tempdir_ = *tempdir_statusor;
  }

  void TearDown() override {
    if (std::filesystem::exists(tempdir_)) {
      std::filesystem::remove_all(tempdir_);
    }
  }

  std::filesystem::path tempdir_;
};

TEST_F(CreateTempdirTest, SuccessfullyCreateFactFile) {
  std::string_view kFileContents = "hello";
  absl::Status result =
      WriteFactsStringToFactsFile(tempdir_, "SomeRelation", kFileContents);
  EXPECT_TRUE(result.ok());
  std::filesystem::path facts_file = tempdir_ / "SomeRelation.facts";
  EXPECT_TRUE(std::filesystem::exists(facts_file));
  EXPECT_TRUE(std::filesystem::is_regular_file(facts_file));
  std::ifstream facts_file_stream(facts_file, std::ios::in | std::ios::binary);
  std::ostringstream read_file_stream;
  // Because this is just a test, we can read the whole file by doing "getline"
  // with a delimiter that should not be present.
  for (std::string line; std::getline(facts_file_stream, line, '\1');) {
    read_file_stream << line;
  }
  EXPECT_EQ(read_file_stream.str(), kFileContents);
}

TEST_F(CreateTempdirTest, WriteFactsToDirectoryTwice) {
  absl::Status result1 =
      WriteFactsStringToFactsFile(tempdir_, "SomeRelation", "");
  EXPECT_TRUE(result1.ok());
  absl::Status result2 =
      WriteFactsStringToFactsFile(tempdir_, "SomeRelation", "");
  EXPECT_FALSE(result2.ok());
  EXPECT_TRUE(IsFailedPrecondition(result2));
}

}  // namespace
}  // namespace raksha::backends::policy_engine::souffle
