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
#include "src/common/utils/types.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"

namespace raksha {
namespace {

TEST(TypesTest, SelectTypeTrue) {
  SelectType<true, int, std::string>::Result a = 3;
  EXPECT_EQ(a, 3);
}

TEST(TypesTest, SelectTypeFalse) {
  SelectType<false, int, std::string>::Result a = "Hi";
  EXPECT_EQ(a, "Hi");
}

TEST(TypesTest, CopyConstTypeTrue) {
  CopyConst<true, int> a = 3;
  // a++; // Should not compile.
  EXPECT_EQ(a, 3);
}

TEST(TypesTest, CopyConstTypeFalse) {
  CopyConst<false, int> a = 4;
  a++;
  EXPECT_EQ(a, 4);
}

}  // namespace
}  // namespace raksha
