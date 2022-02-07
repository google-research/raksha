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
//----------------------------------------------------------------------------
#include "src/ir/operator.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

class OperatorTest : public testing::TestWithParam<absl::string_view> {};

TEST_P(OperatorTest, NameIsCorrectlySet) {
  absl::string_view name = GetParam();
  Operator op(name);
  EXPECT_EQ(op.name(), name);
}

INSTANTIATE_TEST_SUITE_P(OperatorTest, OperatorTest,
                         testing::Values("core.choose", "core.constant",
                                         "arcs.make_particle"));

}  // namespace
}  // namespace raksha::ir
