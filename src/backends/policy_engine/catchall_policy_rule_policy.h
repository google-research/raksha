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
#ifndef SRC_BACKENDS_POLICY_ENGINE_CATCHALL_POLICY_RULE_POLICY_H_
#define SRC_BACKENDS_POLICY_ENGINE_CATCHALL_POLICY_RULE_POLICY_H_

#include <memory>
#include <optional>

#include "souffle/SouffleInterface.h"
#include "absl/log/die_if_null.h"
#include "absl/strings/string_view.h"
#include "src/backends/policy_engine/policy.h"

// A forward declaration of a function created by Souffle
namespace souffle {
SouffleProgram *newInstance_datalog_policy_verifier_cxx();
}

namespace raksha::backends::policy_engine {

// TODO(#728): We ended up in a dependency mess around the SqlPolicyRulePolicy,
// where a number of test frontends started using it that had nothing to do with
// SQL. This is a shim to slim down the SQL infrastructure and allow us to
// eventually eliminate the tests using this as a catchall policy.
class CatchallPolicyRulePolicy : public Policy {
 public:
  explicit CatchallPolicyRulePolicy(std::string is_sql_policy_rule_facts)
      : is_sql_policy_rule_facts_(std::move(is_sql_policy_rule_facts)) {}

  std::unique_ptr<souffle::SouffleProgram> GetPolicyAnalysisChecker()
      const override {
    return std::unique_ptr<souffle::SouffleProgram>(
        ABSL_DIE_IF_NULL(souffle::newInstance_datalog_policy_verifier_cxx()));
  }

  std::optional<std::string> GetPolicyFactName() const override {
    return "isSqlPolicyRule";
  }

  std::optional<std::string> GetPolicyString() const override {
    return is_sql_policy_rule_facts_;
  }

 private:
  std::string is_sql_policy_rule_facts_;
};

}  // namespace raksha::backends::policy_engine

#endif  // SRC_BACKENDS_POLICY_ENGINE_CATCHALL_POLICY_RULE_POLICY_H_
