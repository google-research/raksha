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
#ifndef THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_CHAOTIC_POLICY_CHECKER_H_
#define THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_CHAOTIC_POLICY_CHECKER_H_

#include "absl/strings/string_view.h"
#include "src/backends/policy_engine/chaotic/sql_policy_rules.h"
#include "src/backends/policy_engine/policy.h"
#include "src/backends/policy_engine/policy_checker.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine {

// A policy checker based on computing fixpoints using chaotic iterations.
class ChaoticPolicyChecker : public PolicyChecker {
 public:
  ChaoticPolicyChecker(SqlPolicyRules policy_rules)
      : policy_rules_(policy_rules) {}

  bool IsModulePolicyCompliant(const ir::Module& module,
                               const Policy& policy) const override;

 private:
  bool CheckPolicyCompliance(const ir::Module& module) const;

  SqlPolicyRules policy_rules_;
};

}  // namespace raksha::backends::policy_engine

#endif  // THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_CHAOTIC_POLICY_CHECKER_H_
