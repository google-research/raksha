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

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/strings/match.h"
#include "souffle/SouffleInterface.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/ranges.h"

namespace raksha::ambient {

namespace {

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
        setting << user << usage;
        rel->insert(setting);
      }
    }
  }
}

}  // namespace

bool PolicyChecker::CanUserChangeSetting(absl::string_view user,
                                         absl::string_view setting_name) {
  std::unique_ptr<souffle::SouffleProgram> program(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName));
  CHECK_NOTNULL(program);
  program->run();

  // .decl says_canSay_isEnabled(speaker: Principal, delegatee1: Principal, usage: symbol)
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

absl::flat_hash_set<std::string> PolicyChecker::AvailableSettings(
    absl::string_view user) const {
  absl::flat_hash_set<std::string> result;
  std::unique_ptr<souffle::SouffleProgram> program(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName));
  CHECK_NOTNULL(program);
  program->run();

    // .decl says_canSay_isEnabled(speaker: Principal, delegatee1: Principal, usage: symbol)
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

bool PolicyChecker::ChangeSetting(absl::string_view user,
                                  absl::string_view settings_name, bool value) {
  if (!CanUserChangeSetting(user, settings_name)) return false;
  CHECK(user == kOwnerUser || user == kGuestUser);
  user_settings_[user][settings_name] = value;
  return true;
}

std::pair<bool, std::string> PolicyChecker::ValidatePolicyCompliance() const {
  std::unique_ptr<souffle::SouffleProgram> program(
      souffle::ProgramFactory::newInstance(kPolicyCheckerProgramName));
  CHECK_NOTNULL(program);

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
