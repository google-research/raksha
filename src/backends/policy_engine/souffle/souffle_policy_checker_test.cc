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
#include "src/backends/policy_engine/souffle/souffle_policy_checker.h"

#include "src/common/testing/gtest.h"

namespace raksha::backends::policy_engine {
namespace {

TEST(SoufflePolicyCheckerTest, ReturnsTrue) {
  SoufflePolicyChecker checker;
  ir::Module module;
  Policy policy;
  EXPECT_TRUE(checker.IsModulePolicyCompliant(module, policy));
}

}  // anonymous namespace
}  // namespace raksha::backends::policy_engine
