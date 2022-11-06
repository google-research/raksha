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
#ifndef SRC_BACKENDS_POLICY_ENGINE_AUTH_LOGIC_POLICY_H_
#define SRC_BACKENDS_POLICY_ENGINE_AUTH_LOGIC_POLICY_H_

#include <filesystem>
#include <optional>

#include "absl/strings/string_view.h"
#include "src/backends/policy_engine/policy.h"

namespace raksha::backends::policy_engine {

// Policy interface for authorization logic based policies.
class AuthLogicPolicy : public Policy {
 public:
  explicit AuthLogicPolicy(std::string auth_logic_policy_engine)
      : auth_logic_policy_engine_(std::move(auth_logic_policy_engine)) {}

  // policy checker for authorization logic based policies.
  std::string GetPolicyAnalysisCheckerName() const override {
    return auth_logic_policy_engine_;
  }

  // Policies from authorization logic are converetd to datalog and eventually
  // compiled into analysis, not populated into facts directory. Hence, fact
  // names is empty for auth logic based policies.
  std::optional<std::string> GetPolicyFactName() const override { return {}; }

  // Policy string is empty for auth logic based policies as fact directory are
  // not expected to be populated.
  std::optional<std::string> GetPolicyString() const override { return {}; }

 private:
  std::string auth_logic_policy_engine_;
};

}  // namespace raksha::backends::policy_engine

#endif  // SRC_BACKENDS_POLICY_ENGINE_AUTH_LOGIC_POLICY_H_
