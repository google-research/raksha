/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "src/ir/auth_logic/is_name_constant.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir::auth_logic {

TEST(IsNameConstantTest, BasicTest) {
  EXPECT_EQ(IsNameConstant("\"hello\""), true);
  EXPECT_EQ(IsNameConstant("\"hello"), false);
  EXPECT_EQ(IsNameConstant("hello"), false);
}

};  // namespace raksha::ir::auth_logic
