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

#include "src/common/utils/filesystem.h"

#include "src/common/testing/gtest.h"

namespace raksha::common::utils {

TEST(CreateTemporaryDirectoryTest, CreateTemporaryDirectoryTest) {
  std::filesystem::path temp_dir = CreateTemporaryDirectory();
  EXPECT_TRUE(std::filesystem::exists(temp_dir));
  EXPECT_TRUE(std::filesystem::is_directory(temp_dir));
  EXPECT_TRUE(std::filesystem::is_empty(temp_dir));

  std::filesystem::path temp_dir2 = CreateTemporaryDirectory();
  EXPECT_NE(temp_dir, temp_dir2);
  std::filesystem::remove_all(temp_dir);
  std::filesystem::remove_all(temp_dir2);
}

}  // namespace raksha::common::utils
