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

#include "src/common/testing/gtest.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::datalog {

TEST(ArgumentTypeFactoryTest, SimpleFactoryTest) {
  ArgumentType number = ArgumentType::MakeNumberType();
  ArgumentType principal = ArgumentType::MakePrincipalType();
  ArgumentType custom1 = ArgumentType::MakeCustomType("Custom1");
  ArgumentType custom2 = ArgumentType::MakeCustomType("Custom2");
  EXPECT_EQ(number.name(), "Number");
  EXPECT_EQ(principal.name(), "Principal");
  EXPECT_EQ(custom1.name(), "Custom1");
  EXPECT_EQ(custom2.name(), "Custom2");
  EXPECT_EQ(number.kind(), ArgumentType::Kind::kNumber);
  EXPECT_EQ(principal.kind(), ArgumentType::Kind::kPrincipal);
  EXPECT_EQ(custom1.kind(), ArgumentType::Kind::kCustom);
  EXPECT_EQ(custom2.kind(), ArgumentType::Kind::kCustom);
}

}  // namespace raksha::ir::datalog
