#include "src/analysis/taint/abstract_semantics.h"

#include <optional>
#include <variant>

#include "absl/algorithm/container.h"
#include "src/analysis/taint/abstract_ifc_tags.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/common/utils/fold.h"
#include "src/common/utils/overloaded.h"

namespace raksha::analysis::taint {

namespace {

// Returns true if the index is within the bounds of the given container.
template <typename C>
inline bool IndexInBounds(typename C::size_type index, const C& container) {
  return index >= 0 && index < container.size();
}

}  // namespace

AbstractIfcTags AbstractSemantics::ApplyDefaultOperationTransformer(
    const ir::Operation& operation,
    const std::vector<AbstractIfcTags>& input_states) const {
  // We treat an operation that has no arguments as if it is a seed value.
  if (input_states.empty()) {
    return GetInitialState(
        ir::Value::MakeDefaultOperationResultValue(operation));
  }

  // Computes the join of the inputs.
  AbstractIfcTags joined_result = raksha::common::utils::fold(
      input_states, AbstractIfcTags::Bottom(),
      [](const AbstractIfcTags& so_far, const AbstractIfcTags& cur) {
        return so_far.Join(cur);
      });
  if (joined_result.IsBottom()) return joined_result;
  // We clear the integrity tags as they should not be preserved by default.
  return AbstractIfcTags(joined_result.secrecy_tags(), {});
}

namespace {

// Interprets the merge integrity tags rule and returns the result.
AbstractIfcTags InteprepretMergeIntegrityTags(
    const MergeIntegrityTags& merge_tags,
    const std::vector<AbstractIfcTags>& input_states,
    const AbstractIfcTags& current_value) {  // transform(bottom) = bottom
  if (current_value.IsBottom()) return current_value;
  // If any of the input states are bottom, we return bottom. Note
  // that bottom on an input state means that the specific value has
  // not been processed during the fixpoint computation yet. It is
  // sound to interpret this operation based on the remaining input
  // states. However, we run the risk of computing a less precise
  // fix point. Consider for example an operation for which we need
  // to union the integrity tags:  `z = union.integrity_tags(x, y).`
  // Suppose that we let processing to continue with bottom inputs.
  // Here is a possible sequence of iterations:
  //  iter0: x: bottom,     y: ({}, {I1}),
  //         z: bottom join ({}, {I1}) = ({}, {I1})
  //  iter1: x: ({}, {I0}), y: ({}, {I1}),
  //         z: ({}, {I1}) join ({}, {I0, I1}) = ({}, {I1})
  //
  // On the other hand, if we let the processing wait until all the
  // inputs are non-botom, here is a possible sequence of iterations:
  //  iter0: x: bottom,     y: ({}, {I1}),
  //         z: bottom join bottom = bottom
  //  iter1: x: ({}, {I0}), y: ({}, {I1}),
  //         z: bottom join ({}, {I0, I1}) = ({}, {I0, I1})
  //
  // Note that the latter result is more precise.
  if (absl::c_any_of(input_states, [](const AbstractIfcTags& input_state) {
        return input_state.IsBottom();
      })) {
    return AbstractIfcTags::Bottom();
  }
  TagIdSet current_secrecy_tags = current_value.secrecy_tags();
  auto [first_time, new_integrity_tags] = ::raksha::common::utils::fold(
      merge_tags.input_indices, std::make_pair(true, TagIdSet()),
      [&merge_tags, &input_states](std::pair</*first_time=*/bool,
                                             /*accumulated_tags=*/TagIdSet>
                                       accumulated,
                                   uint64_t index) {
        TagIdSet result;
        CHECK(IndexInBounds(index, input_states))
            << "Index is out of bounds in MergeIntegrityTags";
        const AbstractIfcTags& input_state = input_states[index];
        const auto& [first_time, accumulated_tags] = accumulated;
        const TagIdSet& input_integrity_tags = input_state.integrity_tags();
        if (first_time) {
          return std::make_pair(false, input_integrity_tags);
        }
        switch (merge_tags.kind) {
          case MergeIntegrityTags::Kind::kUnion:
            result = std::move(accumulated_tags);
            absl::c_copy(input_integrity_tags,
                         std::inserter(result, result.end()));
            break;
          case MergeIntegrityTags::Kind::kIntersect:
            for (const TagId& tag : accumulated_tags) {
              if (input_integrity_tags.contains(tag)) {
                result.insert(tag);
              }
            }
            break;
        }
        return std::make_pair(false, result);
      });
  return AbstractIfcTags(current_secrecy_tags, new_integrity_tags);
}
}  // namespace

AbstractIfcTags AbstractSemantics::InterpretInferenceRule(
    const InferenceRule& rule, const ir::Operation& operation,
    const std::vector<AbstractIfcTags>& input_states,
    const AbstractIfcTags& current_value) const {
  bool premises_not_satisfied = absl::c_any_of(
      rule.premises, [&input_states](const InputHasIntegrity& premise) {
        CHECK(IndexInBounds(premise.input_index, input_states))
            << "Premise index is out of bounds. ";
        const AbstractIfcTags& input_state = input_states[premise.input_index];
        // We treat bottom as if all the integrity tags are set.
        return !input_state.IsBottom() &&
               !input_state.HasIntegrity(premise.tag);
      });

  if (premises_not_satisfied) {
    return current_value;
  }

  // Interpret the inference rule if the premises are satisfied.
  return std::visit(
      utils::overloaded{
          [&input_states](const CopyInput& copy_input) {
            CHECK(IndexInBounds(copy_input.input_index, input_states))
                << "Input index in CopyInput is out of bounds";
            return input_states[copy_input.input_index];
          },
          [&current_value](const ModifyTag& modify_tag) {
            // transform(bottom) = bottom
            if (current_value.IsBottom()) return current_value;
            switch (modify_tag.kind) {
              case ModifyTag::Kind::kAddIntegrity:
                return current_value.SetIntegrity(modify_tag.tag);
              case ModifyTag::Kind::kRemoveSecrecy:
                return current_value.ClearSecrecy(modify_tag.tag);
              case ModifyTag::Kind::kAddSecrecy:
                return current_value.SetSecrecy(modify_tag.tag);
            }
          },
          [&current_value,
           &input_states](const MergeIntegrityTags& merge_tags) {
            return InteprepretMergeIntegrityTags(merge_tags, input_states,
                                                 current_value);
          },
          [&current_value](auto value) { return current_value; }},
      rule.conclusion);
}

std::vector<AbstractIfcTags> AbstractSemantics::ApplyOperationTransformer(
    const ir::Operation& operation,
    const std::vector<AbstractIfcTags>& input_states) const {
  AbstractIfcTags default_result =
      ApplyDefaultOperationTransformer(operation, input_states);
  std::vector<AbstractIfcTags> results(operation.NumberOfOutputs(),
                                       default_result);
  auto output_inference_rules_opt =
      inference_rules_.GetOutputRulesForOperation(operation);
  if (output_inference_rules_opt.has_value()) {
    const InferenceRules::OutputRules& output_inference_rules =
        *output_inference_rules_opt;
    for (const auto& [index, inference_rules] : output_inference_rules) {
      CHECK(IndexInBounds(index, results))
          << "Output index in inference rules is out of bounds";

      results[index] = raksha::common::utils::fold(
          inference_rules, AbstractIfcTags(default_result),
          [this, &operation, &input_states](
              AbstractIfcTags accumulated,
              const InferenceRule& inference_rule) {
            return InterpretInferenceRule(inference_rule, operation,
                                          input_states, accumulated);
          });
    }
  }
  return results;
}

AbstractIfcTags AbstractSemantics::GetInitialState(
    const ir::Value& value) const {
  return AbstractIfcTags({}, {});
}

}  // namespace raksha::analysis::taint
