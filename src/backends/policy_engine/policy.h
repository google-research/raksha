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
#ifndef SRC_BACKENDS_POLICY_ENGINE_POLICY_H_
#define SRC_BACKENDS_POLICY_ENGINE_POLICY_H_

#include <filesystem>

#include "absl/status/status.h"

namespace raksha::backends::policy_engine {

// Placeholder for the Policy datastructure. Right now, the policy is hard-coded
// in datalog directly.
class Policy {
 public:
  Policy(std::string is_sql_policy_rule_facts)
      : is_sql_policy_rule_facts_(std::move(is_sql_policy_rule_facts)) {}

  absl::string_view is_sql_policy_rule_facts() const {
    return is_sql_policy_rule_facts_;
  }

 private:
  // At the moment, this is very SQL-verifier specific. We should refactor this
  // once we have a clearer idea of the interface between the verifier and the
  // analysis.
  std::string is_sql_policy_rule_facts_;
};

}  // namespace raksha::backends::policy_engine

#endif
