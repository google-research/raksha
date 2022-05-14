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

#ifndef SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_FACT_POLICY_H_
#define SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_FACT_POLICY_H_

#include <fstream>

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/backends/policy_engine/policy.h"
#include "src/backends/policy_engine/souffle/utils.h"

namespace raksha::backends::policy_engine::souffle {

class DatalogFactPolicy : public Policy {
 public:
  DatalogFactPolicy(absl::string_view relation_name,
                    absl::string_view facts_string)
      : relation_name_(relation_name), facts_string_(facts_string) {}

  absl::Status OutputAsDlFactsToDirectory(
      const std::filesystem::path &output_path) const override {
    return WriteFactsStringToFactsFile(output_path, relation_name_,
                                       facts_string_);
  }

 private:
  std::string relation_name_;
  std::string facts_string_;
};

class SqlPolicyRuleFactPolicy : public DatalogFactPolicy {
 public:
  SqlPolicyRuleFactPolicy(absl::string_view facts_string)
      : DatalogFactPolicy("isSqlPolicyRule", facts_string) {}
};

}  // namespace raksha::backends::policy_engine::souffle

#endif  // SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_FACT_POLICY_H_
