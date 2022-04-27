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

#include "src/backends/policy_engine/souffle/souffle_policy_checker.h"

#include "souffle/SouffleInterface.h"
#include "src/backends/policy_engine/policy.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine {

static constexpr char kDatalogPolicyEngine[] = "datalog_policy_engine_cxx";
static constexpr char kViolatesSqlPolicyRelation[] = "violatesSqlPolicy";

bool SoufflePolicyChecker::IsModulePolicyCompliant(const ir::Module& module,
                                                   const Policy& policy) const {
  std::unique_ptr<souffle::SouffleProgram> program(CHECK_NOTNULL(
      souffle::ProgramFactory::newInstance(std::string(kDatalogPolicyEngine))));

  // TODO: Add relations from module.
  program->run();

  souffle::Relation* hasSqlPolicyViolation =
      CHECK_NOTNULL(program->getRelation(kViolatesSqlPolicyRelation));

  return hasSqlPolicyViolation->size() > 0 ? false : true;
}

}  // namespace raksha::backends::policy_engine
