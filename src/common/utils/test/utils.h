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

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#ifndef SRC_COMMON_UTILS_TEST_UTILS_H_
#define SRC_COMMON_UTILS_TEST_UTILS_H_

namespace raksha::utils::test {
// Typically used to get ROOT DiRECTORY for testing.
// Takes path string as parameter and returns $TEST_SRCDIR/$TEST_WORKSPACE/path
std::filesystem::path GetTestDataDir(std::string path);
// Reads file line by line, stores in a vector and returns it.
std::vector<std::string> ReadFileLines(const std::filesystem::path& file);

}  // namespace raksha::utils::test

#endif  // SRC_COMMON_UTILS_TEST_UTILS_H_
