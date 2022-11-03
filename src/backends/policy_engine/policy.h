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
#include <optional>

#include "absl/strings/string_view.h"

namespace raksha::backends::policy_engine {

// Placeholder for the Policy interface. Right now, the policy is represented
// as a string of datalog facts dumped to a facts file, whereas we will
// probably want something more flexible in the future.
class Policy {
 public:
  virtual ~Policy() = default;

  // The name of the checker that should be used to analyze this kind of policy.
  // Should return a constant string per policy subclass.
  // This is here because the kind of checker used is an aspect of the policy:
  // when a policy is too complex to fit into simple data facts, it is compiled
  // into the analysis itself. Thus, it is only appropriate to place the checker
  // to be called into the `Policy` as if the data compiled in were opaque
  // members of this object.
  virtual std::string GetPolicyAnalysisCheckerName() const = 0;

  // Gets the name of the Datalog fact populated by this policy. If this policy
  // does not populate any facts, (it is all compiled into the analysis) this
  // can be absent.
  virtual std::optional<std::string> GetPolicyFactName() const = 0;

  // Gets the string representation of the datalog facts included in this
  // policy. If this policy does not populate any facts, (it is all compiled
  // into the analysis) this can be absent.
  virtual std::optional<std::string> GetPolicyString() const = 0;
};

}  // namespace raksha::backends::policy_engine

#endif
