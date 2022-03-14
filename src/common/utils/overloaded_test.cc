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
#include "src/common/utils/overloaded.h"

#include <string>
#include <iostream>

#include "src/common/testing/gtest.h"

namespace raksha::utils {

enum class Kind { kInt, kLong, kDouble, kString } kind;
using VariantType = std::variant<int, long, double, std::string>;

class OverloadedTest
    : public testing::TestWithParam<std::pair<VariantType, Kind>> {};

TEST_P(OverloadedTest, DispatchesToCorrectType) {
  const auto& [variant, expected_kind] = GetParam();
  Kind kind =
      std::visit(overloaded{[](int l) { return Kind::kInt; },
                            [](long l) { return Kind::kLong; },
                            [](const std::string& s) { return Kind::kString; },
                            [](double d) { return Kind::kDouble; }},
                 variant);
  EXPECT_EQ(kind, expected_kind);
}

INSTANTIATE_TEST_SUITE_P(OverloadedTest, OverloadedTest,
                         testing::Values(std::make_pair(10, Kind::kInt),
                                         std::make_pair(2.0, Kind::kDouble),
                                         std::make_pair(5L, Kind::kLong),
                                         std::make_pair("hello",
                                                        Kind::kString)));

}  // namespace raksha::utils
