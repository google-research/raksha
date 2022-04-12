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

#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "souffle/SouffleInterface.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/ranges.h"

namespace raksha::ambient {

namespace {

static constexpr char kASRSetting[] = "ASR";
static constexpr char kStreamingSetting[] = "Streaming";
static constexpr char kRecordingSetting[] = "Recording";
static constexpr char kOwnerUser[] = "OwnerUser";
static constexpr char kGuestUser[] = "GuestUser";
static constexpr char kMicrophoneAudio[] = "R.SmartMicrophone.audio";
static constexpr char kMicrophoneOut[] = "R.mic_audio_stream_out";
static constexpr char kDemoAppIn[] = "R.app_audio_stream_in";
static constexpr char kDemoAppOut[] = "R.app_audio_stream_out";
static constexpr char kDemoASRIn[] = "R.asr_audio_stream_in";
static constexpr char kDemoStoreIn[] = "R.store_audio_stream_in";
static constexpr char kSystemSettingsManager[] = "SystemSettingsManager";

absl::string_view GetSettingsName(PolicyChecker::Settings settings) {
  switch (settings) {
    case PolicyChecker::Settings::kASR: {
      return kASRSetting;
    }
    case PolicyChecker::Settings::kStreaming: {
      return kStreamingSetting;
    }
    case PolicyChecker::Settings::kRecording: {
      return kRecordingSetting;
    }
    default: {
      CHECK(false) << "Unknown settings";
      // This is unreachable, but adding a return to keep the compiler happy.
      return "";
    }
  }
}

PolicyChecker::Settings GetSettings(absl::string_view name) {
  if (name == kASRSetting) {
    return PolicyChecker::Settings::kASR;
  } else if (name == kStreamingSetting) {
    return PolicyChecker::Settings::kStreaming;
  } else if (name == kRecordingSetting) {
    return PolicyChecker::Settings::kRecording;
  } else {
    CHECK(false) << "Unknown settings";
    // This is unreachable, but adding a return to keep the compiler happy.
    return PolicyChecker::Settings::kUnknown;
  }
}

absl::string_view GetUserName(PolicyChecker::User user) {
  switch (user) {
    case PolicyChecker::User::kOwner: {
      return kOwnerUser;
    }
    case PolicyChecker::User::kGuest: {
      return kGuestUser;
    }
    default: {
      CHECK(false) << "Unknown user";
      // This is unreachable, but adding a return to keep the compiler happy.
      return "";
    }
  }
}

absl::string_view GetDataConnectionName(PolicyChecker::Node node,
                                        bool in_bound) {
  switch (node) {
    case PolicyChecker::Node::kSmartMicrophone: {
      CHECK(!in_bound) << "No inbound connections for SmartMicrophone";
      return kMicrophoneOut;
    }
    case PolicyChecker::Node::kDemoApplication: {
      return in_bound ? kDemoAppIn : kDemoAppOut;
    }
    case PolicyChecker::Node::kDemoApplicationASR: {
      CHECK(in_bound) << "No outbound connections for ASR application.";
      return kDemoASRIn;
    }
    case PolicyChecker::Node::kDemoApplicationAudioStore: {
      CHECK(in_bound) << "No outbound connections for ASR application.";
      return kDemoStoreIn;
    }
    case PolicyChecker::Node::kUnknown:
    default: {
      CHECK(false) << "Unknown node";
      // This is unreachable, but adding a return to keep the compiler happy.
      return "";
    }
  }
}

template <typename Range>
void UpdateEdges(const souffle::SouffleProgram *program, Range edges) {
  souffle::Relation *rel = CHECK_NOTNULL(program->getRelation("edge"));
  for (const auto &[src, tgt] : edges) {
    souffle::tuple edge(rel);  // Create an empty tuple
    edge << std::string(src) << std::string(tgt);
    rel->insert(edge);
  }
}

template <typename Range>
void UpdateSettings(const souffle::SouffleProgram *program,
                    Range user_settings) {
  souffle::Relation *rel =
      CHECK_NOTNULL(program->getRelation("says_isEnabled"));
  // .decl hasTag(accessPath: AccessPath, owner: Principal, tag: Tag)
  for (const auto &[user, settings] : user_settings) {
    for (const auto &[usage, allowed] : settings) {
      souffle::tuple setting(rel);  // Create an empty tuple
      if (allowed) {
        setting << std::string(GetUserName(user))
                << std::string(GetSettingsName(usage));
        rel->insert(setting);
      }
    }
  }
}

}  // namespace

bool PolicyChecker::CanUserChangeSetting(PolicyChecker::User user,
                                         PolicyChecker::Settings setting) {
  return CanUserChangeSetting(GetUserName(user), GetSettingsName(setting));
}

bool PolicyChecker::CanUserChangeSetting(absl::string_view user,
                                         absl::string_view setting_name) {
  std::unique_ptr<souffle::SouffleProgram> program(CHECK_NOTNULL(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName)));
  program->run();

  // .decl says_canSay_isEnabled(speaker: Principal, delegatee1: Principal,
  // usage: symbol)
  souffle::Relation *saysCanSayIsEnabled =
      CHECK_NOTNULL(program->getRelation("says_canSay_isEnabled"));
  // std::string setting_tag = absl::StrFormat("Allow%s", setting_name);
  for (auto &output : *saysCanSayIsEnabled) {
    std::string speaker, delegatee1, usage;
    output >> speaker >> delegatee1 >> usage;
    if (speaker == kSystemSettingsManager && delegatee1 == user &&
        usage == setting_name) {
      return true;
    }
  }
  return false;
}

absl::flat_hash_set<PolicyChecker::Settings> PolicyChecker::AvailableSettings(
    PolicyChecker::User user) const {
  absl::flat_hash_set<PolicyChecker::Settings> result;
  for (auto name : AvailableSettings(GetUserName(user))) {
    result.insert(GetSettings(name));
  }
  return result;
}

absl::flat_hash_set<std::string> PolicyChecker::AvailableSettings(
    absl::string_view user) const {
  absl::flat_hash_set<std::string> result;
  std::unique_ptr<souffle::SouffleProgram> program(CHECK_NOTNULL(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName)));
  program->run();

  // .decl says_canSay_isEnabled(speaker: Principal, delegatee1: Principal,
  // usage: symbol)
  souffle::Relation *saysCanSayIsEnabled =
      CHECK_NOTNULL(program->getRelation("says_canSay_isEnabled"));
  // std::string setting_tag = absl::StrFormat("Allow%s", setting_name);
  for (auto &output : *saysCanSayIsEnabled) {
    std::string speaker, delegatee1, usage;
    output >> speaker >> delegatee1 >> usage;
    if (speaker == kSystemSettingsManager && delegatee1 == user) {
      result.insert(usage);
    }
  }
  return result;
}

bool PolicyChecker::ChangeSetting(PolicyChecker::User user,
                                  PolicyChecker::Settings setting, bool value) {
  absl::string_view user_name = GetUserName(user);
  absl::string_view settings_name = GetSettingsName(setting);
  if (!CanUserChangeSetting(user_name, settings_name)) {
    return false;
  }
  CHECK(user == User::kOwner || user == User::kGuest);
  user_settings_[user][setting] = value;
  return true;
}

std::pair<bool, std::string> PolicyChecker::AddIfValidEdge(Node source,
                                                           Node target) {
  return AddIfValidEdge(GetDataConnectionName(source, /*inbound=*/false),
                        GetDataConnectionName(target, /*inbound=*/true));
}

std::pair<bool, std::string> PolicyChecker::AddIfValidEdge(
    absl::string_view src, absl::string_view tgt) {
  bool edge_already_in_context = IsEdgePresent(src, tgt);
  if (!edge_already_in_context) AddEdge(src, tgt);
  auto result = ValidatePolicyCompliance();
  if (!result.first && !edge_already_in_context) {
    RemoveEdge(src, tgt);
  }
  return result;
}

std::pair<bool, std::string> PolicyChecker::ValidatePolicyCompliance() const {
  std::unique_ptr<souffle::SouffleProgram> program(CHECK_NOTNULL(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName)));

  UpdateEdges(program.get(), utils::make_range(edges_.begin(), edges_.end()));
  UpdateSettings(program.get(), utils::make_range(user_settings_.begin(),
                                                  user_settings_.end()));

  program->run();

  souffle::Relation *disallowed_usage =
      CHECK_NOTNULL(program->getRelation("disallowedUsage"));
  // .decl disallowedUsage(dataConsumer: Principal, usage: Usage, owner:
  // Principal, tag: Tag)

  if (disallowed_usage->size() == 0) {
    return std::make_pair(true, "Success");
  }

  std::string data_consumer, usage, owner, tag;
  std::string reason("Disallowed Usages:\n");
  for (auto &output : *disallowed_usage) {
    output >> data_consumer >> usage >> owner >> tag;
    absl::StrAppend(&reason, absl::StrFormat(" '%s' for '%s' of '%s'\n", usage,
                                             tag, owner));
  }
  return std::make_pair(false, reason);
}

}  // namespace raksha::ambient
