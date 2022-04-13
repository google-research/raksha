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

class PolicyCheckerEdgeTest : public testing::TestWithParam<bool> {};

TEST_P(PolicyCheckerEdgeTest, StreamingEdgeTest) {
  bool setting = GetParam();
  PolicyChecker checker;
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kStreaming, setting);
  {
    const auto& [is_compliant, reason] =
        checker.IsValidEdge(PolicyChecker::Node::kSmartMicrophone,
                            PolicyChecker::Node::kDemoApplication);
    EXPECT_EQ(is_compliant, setting)
        << "Edge kMicrophoneOut -> kDemoAppIn Invalid:\n"
        << reason;
  }
}

TEST_P(PolicyCheckerEdgeTest, ASREdgeTest) {
  bool setting = GetParam();
  PolicyChecker checker;
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kASR, setting);
  const auto& [is_compliant, reason] =
      checker.IsValidEdge(PolicyChecker::Node::kDemoApplication,
                          PolicyChecker::Node::kDemoApplicationASR);
  EXPECT_EQ(is_compliant, setting)
      << "Edge kDemoAppOut -> kDemoASRIn Invalid:\n"
      << reason;
}

TEST_P(PolicyCheckerEdgeTest, StorageEdgeTest) {
  bool setting = GetParam();
  PolicyChecker checker;
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kRecording, setting);
  const auto& [is_compliant, reason] =
      checker.IsValidEdge(PolicyChecker::Node::kDemoApplication,
                          PolicyChecker::Node::kDemoApplicationAudioStore);
  EXPECT_EQ(is_compliant, setting)
      << "Edge kDemoAppOut -> kDemoStoreIn Invalid:\n"
      << reason;
}

INSTANTIATE_TEST_SUITE_P(PolicyCheckerEdgeTest, PolicyCheckerEdgeTest,
                         testing::Values(false, true));

TEST(PolicyCheckerTest, EdgesTest) {
  PolicyChecker checker;
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kASR, true);
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kRecording, true);
  checker.ChangeSetting(PolicyChecker::User::kOwner,
                        PolicyChecker::Settings::kStreaming, true);
  {
    const auto& [is_compliant, reason] =
        checker.AddIfValidEdge(PolicyChecker::Node::kSmartMicrophone,
                               PolicyChecker::Node::kDemoApplication);

    EXPECT_EQ(is_compliant, true)
        << "Edge kMicrophoneOut -> kDemoAppIn Invalid:\n"
        << reason;
  }
  {
    const auto& [is_compliant, reason] =
        checker.AddIfValidEdge(PolicyChecker::Node::kDemoApplication,
                               PolicyChecker::Node::kDemoApplicationASR);
    EXPECT_EQ(is_compliant, true) << "Edge kDemoAppOut -> kDemoASRIn Invalid:\n"
                                  << reason;
  }
  {
    const auto& [is_compliant, reason] =
        checker.AddIfValidEdge(PolicyChecker::Node::kDemoApplication,
                               PolicyChecker::Node::kDemoApplicationAudioStore);
    EXPECT_EQ(is_compliant, true)
        << "Edge kDemoAppOut -> kDemoStoreIn Invalid:\n"
        << reason;
  }
  const auto& [is_compliant, reason] = checker.ValidatePolicyCompliance();
  EXPECT_EQ(is_compliant, true) << "Policy failing:\n" << reason;
}

TEST(PolicyCheckerTest, WhoCanChangeSettings) {
  PolicyChecker checker;
  EXPECT_TRUE(checker.CanUserChangeSetting(PolicyChecker::User::kOwner,
                                           PolicyChecker::Settings::kASR));
  EXPECT_TRUE(checker.CanUserChangeSetting(
      PolicyChecker::User::kOwner, PolicyChecker::Settings::kRecording));
  EXPECT_TRUE(checker.CanUserChangeSetting(
      PolicyChecker::User::kOwner, PolicyChecker::Settings::kStreaming));
  EXPECT_FALSE(checker.CanUserChangeSetting(PolicyChecker::User::kGuest,
                                            PolicyChecker::Settings::kASR));
  EXPECT_FALSE(checker.CanUserChangeSetting(
      PolicyChecker::User::kGuest, PolicyChecker::Settings::kRecording));
  EXPECT_FALSE(checker.CanUserChangeSetting(
      PolicyChecker::User::kGuest, PolicyChecker::Settings::kStreaming));
}

TEST(PolicyCheckerTest, AvailableSettings) {
  PolicyChecker checker;
  EXPECT_THAT(
      checker.AvailableSettings(PolicyChecker::User::kOwner),
      testing::UnorderedElementsAre(PolicyChecker::Settings::kASR,
                                    PolicyChecker::Settings::kRecording,
                                    PolicyChecker::Settings::kStreaming));
  EXPECT_THAT(checker.AvailableSettings(PolicyChecker::User::kGuest),
              testing::UnorderedElementsAre());
}

}  // namespace
}  // namespace raksha::ambient
