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
#ifndef SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_SOUFFLE_POLICY_CHECKER_H_
#define SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_SOUFFLE_POLICY_CHECKER_H_

#include "src/backends/policy_engine/policy.h"
#include "src/backends/policy_engine/policy_checker.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine {

class SoufflePolicyChecker : public PolicyChecker {
 public:
  bool IsModulePolicyCompliant(const ir::Module& module,
                               const Policy& policy) const override;
};

}  // namespace raksha::backends::policy_engine

#endif  // SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_SOUFFLE_POLICY_CHECKER_H_
