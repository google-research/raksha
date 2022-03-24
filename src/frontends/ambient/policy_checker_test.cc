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
//
// Policy checker for the multimic use case.
//
#include "src/frontends/ambient/policy_checker.h"

#include <iostream>
#include <memory>

#include "src/common/testing/gtest.h"

namespace raksha::ambient {
namespace {

TEST(PolicyCheckerTest, EdgesTest) {
  PolicyChecker checker;
  checker.ChangeSetting(PolicyChecker::kOwnerUser, PolicyChecker::kASRSetting,
                        true);
  checker.ChangeSetting(PolicyChecker::kOwnerUser,
                        PolicyChecker::kRecordingSetting, true);
  checker.ChangeSetting(PolicyChecker::kOwnerUser,
                        PolicyChecker::kStreamingSetting, true);
  {
    const auto& [is_compliant, reason] = checker.AddIfValidEdge(
        PolicyChecker::kMicrophoneOut, PolicyChecker::kDemoAppIn);
    EXPECT_EQ(is_compliant, true)
        << "Edge kMicrophoneOut -> kDemoAppIn Invalid:\n"
        << reason;
  }
  {
    const auto& [is_compliant, reason] = checker.AddIfValidEdge(
        PolicyChecker::kDemoAppOut, PolicyChecker::kDemoASRIn);
    EXPECT_EQ(is_compliant, true)
        << "Edge kDemoAppOut -> kDemoASRIn Invalid:\n" << reason;
  }
  {
    const auto& [is_compliant, reason] = checker.AddIfValidEdge(
        PolicyChecker::kDemoAppOut, PolicyChecker::kDemoStoreIn);
    EXPECT_EQ(is_compliant, true)
        << "Edge kDemoAppOut -> kDemoStoreIn Invalid:\n" << reason;
  }
  const auto& [is_compliant, reason] = checker.ValidatePolicyCompliance();
  EXPECT_EQ(is_compliant, true) << "Policy failing:\n" << reason;
}

TEST(PolicyCheckerTest, WhoCanChangeSettings) {
  PolicyChecker checker;
  EXPECT_TRUE(checker.CanUserChangeSetting(PolicyChecker::kOwnerUser,
                                           PolicyChecker::kASRSetting));
  EXPECT_TRUE(checker.CanUserChangeSetting(PolicyChecker::kOwnerUser,
                                           PolicyChecker::kRecordingSetting));
  EXPECT_TRUE(checker.CanUserChangeSetting(PolicyChecker::kOwnerUser,
                                           PolicyChecker::kStreamingSetting));
  EXPECT_FALSE(checker.CanUserChangeSetting(PolicyChecker::kGuestUser,
                                            PolicyChecker::kASRSetting));
  EXPECT_FALSE(checker.CanUserChangeSetting(PolicyChecker::kGuestUser,
                                           PolicyChecker::kRecordingSetting));
  EXPECT_FALSE(checker.CanUserChangeSetting(PolicyChecker::kGuestUser,
                                            PolicyChecker::kStreamingSetting));
}


TEST(PolicyCheckerTest, AvailableSettings) {
  PolicyChecker checker;
  EXPECT_THAT(checker.AvailableSettings(PolicyChecker::kOwnerUser),
              testing::UnorderedElementsAre(PolicyChecker::kASRSetting,
                                            PolicyChecker::kRecordingSetting,
                                            PolicyChecker::kStreamingSetting));
  EXPECT_THAT(checker.AvailableSettings(PolicyChecker::kGuestUser),
              testing::UnorderedElementsAre());
}

}  // namespace
}  // namespace raksha::ambient
