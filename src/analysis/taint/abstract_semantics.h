#ifndef SRC_ANALYSIS_TAINT_ABSTRACT_SEMANTICS_H_
#define SRC_ANALYSIS_TAINT_ABSTRACT_SEMANTICS_H_

#include <variant>

#include "absl/container/flat_hash_set.h"
#include "src/analysis/common/module_graph.h"
#include "src/analysis/taint/abstract_ifc_tags.h"
#include "src/analysis/taint/inference_rules.h"

namespace raksha::analysis::taint {

class AbstractSemantics {
 public:
  using Graph = common::ModuleGraph;
  using AbstractState = AbstractIfcTags;

  explicit AbstractSemantics(InferenceRules inference_rules)
      : inference_rules_(std::move(inference_rules)) {}

  // Returns the inital state for the given value.
  AbstractIfcTags GetInitialState(const ir::Value& value) const;

  // Applies the operation semantics on the inputs and returns the results.
  std::vector<AbstractIfcTags> ApplyOperationTransformer(
      const ir::Operation& operation,
      const std::vector<AbstractIfcTags>& input_states) const;

 private:
  // Applies default semantics for an operation.
  AbstractIfcTags ApplyDefaultOperationTransformer(
      const ir::Operation& operation,
      const std::vector<AbstractIfcTags>& input_states) const;

  // Interprets the inference rule for the operation and returns the result.
  AbstractIfcTags InterpretInferenceRule(
      const InferenceRule& rule, const ir::Operation& operation,
      const std::vector<AbstractIfcTags>& input_states,
      const AbstractIfcTags& current_value) const;

  InferenceRules inference_rules_;
};

}  // namespace raksha::analysis::taint

#endif  // SRC_ANALYSIS_TAINT_SEMANTICS_H_
