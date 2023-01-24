#ifndef SRC_BACKENDS_POLICY_ENGINE_ABSTRACT_INTERPRETATION_ABSTRACT_INTERPRETATION_POLICY_CHECKER_H_
#define SRC_BACKENDS_POLICY_ENGINE_ABSTRACT_INTERPRETATION_ABSTRACT_INTERPRETATION_POLICY_CHECKER_H_

#include "src/analysis/common/module_fixpoint_iterator.h"
#include "src/analysis/taint/abstract_semantics.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/backends/policy_engine/policy.h"
#include "src/backends/policy_engine/policy_checker.h"

namespace raksha::backends::policy_engine {

using TaintAnalysis = raksha::analysis::common::ModuleFixpointIterator<
    analysis::taint::AbstractSemantics>;

class AbstractInterpretationPolicyChecker : public PolicyChecker {
 public:
  AbstractInterpretationPolicyChecker(
      analysis::taint::InferenceRules inference_rules,
      absl::flat_hash_set<std::string> egress_operation_names)
      : inference_rules_(std::move(inference_rules)),
        egress_operation_names_(std::move(egress_operation_names)) {}

  bool IsModulePolicyCompliant(const ir::Module& module,
                               const Policy& policy) const override;

  // Returns the IfcTags for the values in module by performing taint analysis.
  TaintAnalysis::ValueStateMap ComputeIfcTags(const ir::Module& module) const;

 private:
  analysis::taint::InferenceRules inference_rules_;
  absl::flat_hash_set<std::string> egress_operation_names_;
};

}  // namespace raksha::backends::policy_engine

#endif  // SRC_BACKENDS_POLICY_ENGINE_ABSTRACT_INTERPRETATION_ABSTRACT_INTERPRETATION_POLICY_CHECKER_H_
