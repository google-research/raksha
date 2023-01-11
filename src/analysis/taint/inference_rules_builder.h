//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
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
#ifndef SRC_ANALYSIS_TAINT_INFERENCE_RULES_BUILDER_H_
#define SRC_ANALYSIS_TAINT_INFERENCE_RULES_BUILDER_H_

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/ir/module.h"

namespace raksha::analysis::taint {

// A class to build InferenceRules.
class InferenceRulesBuilder {
 public:
  // Constructs a builder by populating the tags names with given seed tags.
  //
  // The ids of the seed tags are in the order in which they appear in the
  // seed_tag_names. Construction fails if there are duplicates entries among
  // the seed tags.
  explicit InferenceRulesBuilder(std::vector<std::string> seed_tag_names = {})
      : tag_names_(std::move(seed_tag_names)) {
    // Populates tag ids in the order of tag_names_.
    for (const std::string& tag_name : tag_names_) {
      auto [_, success] = tags_.insert({tag_name, tags_.size()});
      CHECK(success) << "Duplicate seed tag '" << tag_name << "'";
    }
  }

  // Returns InferenceRules on the current state of the builder object. Note
  // that the builder instance becomes invalid after the invocation of
  // `Build` and should not be used futher.
  InferenceRules Build() {
    return InferenceRules(std::move(action_rules_), std::move(actions_),
                          std::move(tags_), std::move(tag_names_));
  }

  // Marks the given operations as the given action. Fails if the
  // operation is already marked as another action.
  void MarkOperationAsAction(const ir::Operation& operation,
                             absl::string_view action) {
    auto [_, success] = actions_.insert({&operation, std::string(action)});
    CHECK(success) << "Duplicate operation inserted for '" << action << "'";
  }

  // Adds the inference rules for the outputs of the given action.  Fails if
  // there is already an entry associated with the given action name.
  void AddOutputRulesForAction(absl::string_view action,
                               InferenceRules::OutputRules rule) {
    auto [_, success] =
        action_rules_.insert({std::string(action), std::move(rule)});
    CHECK(success) << "Duplicate inference rule inserted for '" << action
                   << "'";
  }

  // Returns the canonical TagId value for the given string.
  TagId GetOrCreateTagId(absl::string_view tag) {
    auto insert_result = tags_.insert({std::string(tag), tags_.size()});
    if (insert_result.second) {
      // Remember the tag name if a new entry was added.
      tag_names_.push_back(insert_result.first->first);
    }
    return insert_result.first->second;
  }

 private:
  // TagIds Table
  std::vector<std::string> tag_names_;
  absl::flat_hash_map<std::string, TagId> tags_;
  // Actions and rules
  InferenceRules::ActionRules action_rules_;
  absl::flat_hash_map<const ir::Operation*, std::string> actions_;
};
}  // namespace raksha::analysis::taint

#endif  // SRC_ANALYSIS_TAINT_INFERENCE_RULES_BUILDER_H_
