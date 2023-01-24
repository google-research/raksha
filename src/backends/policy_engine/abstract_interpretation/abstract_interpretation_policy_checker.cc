#include "src/backends/policy_engine/abstract_interpretation/abstract_interpretation_policy_checker.h"

#include "src/ir/module.h"

namespace raksha::backends::policy_engine {

TaintAnalysis::ValueStateMap
AbstractInterpretationPolicyChecker::ComputeIfcTags(
    const ir::Module& module) const {
  return TaintAnalysis().ComputeFixpoint(
      module,
      /*semantics_maker=*/[this](const ir::Module& module) {
        return analysis::taint::AbstractSemantics(inference_rules_);
      });
}

bool AbstractInterpretationPolicyChecker::IsModulePolicyCompliant(
    const ir::Module& module, const Policy& policy) const {
  auto fixpoint_result = ComputeIfcTags(module);
  // Check that all egresses don't have any secrecy tags.
  bool policy_compliant = true;
  for (const auto& block : module.blocks()) {
    for (const auto& operation : block->operations()) {
      if (!egress_operation_names_.contains(operation->op().name())) continue;
      for (const auto& input : operation->inputs()) {
        auto find_result = fixpoint_result.find(input);
        // No result means this value was unreachable.
        if (find_result == fixpoint_result.end()) continue;
        if (!find_result->second.HasNoSecrecy()) {
          policy_compliant = false;
        }
      }
    }
  }
  return policy_compliant;
}

}  // namespace raksha::backends::policy_engine
