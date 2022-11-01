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
#ifndef SRC_FRONTENDS_AMBIENT_POLICY_CHECKER_H_
#define SRC_FRONTENDS_AMBIENT_POLICY_CHECKER_H_

#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "src/common/containers/hash_set.h"
#include "src/common/containers/hash_map.h"

namespace raksha::ambient {

class PolicyChecker {
 public:
  enum class Node {
    kUnknown,
    kSmartMicrophone,
    kDemoApplication,
    kDemoApplicationASR,
    kDemoApplicationAudioStore,
  };

  enum class User {
    kUnknown,
    kOwner,
    kGuest,
  };

  enum class Settings { kUnknown, kStreaming, kASR, kRecording };

  using DataFlowEdge = std::pair<std::string, std::string>;
  using SettingsMap = common::containers::HashMap<Settings, bool>;

  PolicyChecker() {
    user_settings_[User::kOwner] = SettingsMap({{Settings::kASR, true},
                                                {Settings::kStreaming, true},
                                                {Settings::kRecording, true}});
    user_settings_[User::kGuest] = SettingsMap({{Settings::kASR, true},
                                                {Settings::kStreaming, true},
                                                {Settings::kRecording, true}});
  }

  // Check and add if the given edge can be added safely to the policy context.
  std::pair<bool, std::string> AddIfValidEdge(Node source, Node target);
  // Simply check if the given edge can be added safely to the policy context.
  std::pair<bool, std::string> IsValidEdge(Node source, Node target);

  // If the current setup is policy compliant, returns true. Otherwise, returns
  // false along with a reason.
  std::pair<bool, std::string> ValidatePolicyCompliance() const;

  // If the setting is successful, returns true. Otherwise, returns false.
  bool ChangeSetting(User user, Settings name, bool value);

  // Returns true if the user can change the given setting.
  bool CanUserChangeSetting(User user, Settings setting_name);

  // Returns the set of available settings for the given user.
  common::containers::HashSet<PolicyChecker::Settings> AvailableSettings(
      User user) const;

 private:
  // Check and add if the given edge can be added safely to the policy context.
  std::pair<bool, std::string> AddIfValidEdge(std::string_view src,
                                              std::string_view tgt);
  // Simply check if the given edge can be added safely to the policy context.
  std::pair<bool, std::string> IsValidEdge(std::string_view src,
                                           std::string_view tgt);

  // Returns true if the user can change the given setting.
  bool CanUserChangeSetting(std::string_view user,
                            std::string_view setting_name);

  // Returns the set of available settings for the given user.
  common::containers::HashSet<std::string> AvailableSettings(
      std::string_view user) const;

  // Adds the given edge into the policy context.
  void AddEdge(std::string_view src, std::string_view tgt) {
    edges_.insert(DataFlowEdge(std::string(src), std::string(tgt)));
  }

  // Returns true if the given edge is present in the policy context.
  bool IsEdgePresent(std::string_view src, std::string_view tgt) {
    return edges_.count(DataFlowEdge(std::string(src), std::string(tgt))) > 0;
  }

  // Removes the given edge from the policy context.
  void RemoveEdge(std::string_view src, std::string_view tgt) {
    edges_.erase(DataFlowEdge(std::string(src), std::string(tgt)));
  }

  common::containers::HashSet<DataFlowEdge> edges_;
  common::containers::HashMap<User, SettingsMap> user_settings_;

  static constexpr char kPolicyCheckerProgramName[] =
      "ambient_policy_checker_cxx";
};

}  // namespace raksha::ambient

#endif  // SRC_FRONTENDS_AMBIENT_POLICY_CHECKER_H_
