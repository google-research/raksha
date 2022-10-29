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

#include "src/frontends/sql/driver.h"

#include "src/backends/policy_engine/chaotic/chaotic_policy_checker.h"
#include "src/backends/policy_engine/chaotic/sql_policy_rules.h"
#include "src/backends/policy_engine/policy.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/sql_ir.pb.h"

namespace raksha::frontends::sql {

bool verify(const ExpressionArena &arena,
            const backends::policy_engine::Policy &policy) {
  ir::IRContext ir_context;
  DecoderContext decoder_context(ir_context);
  DecodeExpressionArena(arena, decoder_context);
  decoder_context.BuildTopLevelBlock();
  const ir::Module &module = decoder_context.global_module();
  // LOG(ERROR) << arena.DebugString();
  backends::policy_engine::SqlPolicyRules policy_rules(arena);
  backends::policy_engine::ChaoticPolicyChecker policy_checker(policy_rules);
  return policy_checker.IsModulePolicyCompliant(module, policy);
  //   return backends::policy_engine::SoufflePolicyChecker()
  //       .IsModulePolicyCompliant(module, policy);
}

}  // namespace raksha::frontends::sql
