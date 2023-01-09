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
#ifndef SRC_ANALYSIS_TAINT_INFERENCE_RULES_H_
#define SRC_ANALYSIS_TAINT_INFERENCE_RULES_H_

#include <functional>
#include <utility>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "src/analysis/taint/inference_rule.h"
#include "src/ir/module.h"

namespace raksha::analysis::taint {

// A class to associated taint-related inference rules with IR operations.
class InferenceRules {
 public:
  using InferenceRuleSequence = std::vector<InferenceRule>;
  // A map from output index of an IR operation to the sequence of inference
  // rules that should be applied to get the resulting tags.
  using OutputRules = absl::flat_hash_map<uint64_t, InferenceRuleSequence>;
  using OptionOutputRulesRef =
      std::optional<std::reference_wrapper<const OutputRules>>;
  // A map from actions to the inference rules for the outputs of an action.
  using ActionRules = absl::flat_hash_map<std::string, OutputRules>;

  // A way to construct the inference rules directly. Typically, it is more
  // convenient to constructing this using the InferenceRulesBuilder class.
  InferenceRules(ActionRules action_rules,
                 absl::flat_hash_map<const ir::Operation*, std::string> actions,
                 absl::flat_hash_map<std::string, TagId> tags,
                 std::vector<std::string> tag_names)
      : action_rules_(std::move(action_rules)),
        actions_(std::move(actions)),
        tags_(std::move(tags)),
        tag_names_(std::move(tag_names)) {}

  const ActionRules& action_rules() const { return action_rules_; }
  const absl::flat_hash_map<const ir::Operation*, std::string>& actions()
      const {
    return actions_;
  }
  const absl::flat_hash_map<std::string, TagId>& tags() const { return tags_; }
  const std::vector<std::string>& tag_names() const { return tag_names_; }

  // Returns the canonical TagId value for the given string. If the tag is not
  // known, returns std::nullopt.
  std::optional<TagId> GetTagId(absl::string_view tag) const {
    auto find_result = tags_.find(std::string(tag));
    return find_result == tags_.end()
               ? std::nullopt
               : std::optional<TagId>(find_result->second);
  }

  // Returns the name associated with the given tag id. If the tag id is not
  // known, returns std::nullopt.
  std::optional<absl::string_view> GetTagName(TagId tag) const {
    return (tag < tag_names_.size())
               ? std::optional<absl::string_view>(tag_names_[tag])
               : std::nullopt;
  }

  // Returns the rule associated with the given action.
  OptionOutputRulesRef GetOutputRulesForAction(absl::string_view action) const {
    auto find_result = action_rules_.find(action);
    return (find_result == action_rules_.end())
               ? std::nullopt
               : OptionOutputRulesRef(std::cref(find_result->second));
  }

  // Returns inference rule for the given operation. Returns std::nullopt if
  // there are no inference rules associated with this operation.
  OptionOutputRulesRef GetOutputRulesForOperation(
      const ir::Operation& operation) const {
    auto find_result = actions_.find(&operation);
    return (find_result == actions_.end())
               ? std::nullopt
               : GetOutputRulesForAction(find_result->second);
  }

 private:
  ActionRules action_rules_;
  absl::flat_hash_map<const ir::Operation*, std::string> actions_;

  // TagIds Table
  absl::flat_hash_map<std::string, TagId> tags_;
  std::vector<std::string> tag_names_;
};

}  // namespace raksha::analysis::taint

#endif  // SRC_ANALYSIS_TAINT_INFERENCE_RULES_H_
