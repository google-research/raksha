#include "src/frontends/sql/decode_sql_policy_rules.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/analysis/taint/inference_rules_builder.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/frontends/sql/sql_ir.pb.h"

namespace raksha::frontends::sql {

using analysis::taint::InferenceRule;
using analysis::taint::InferenceRules;
using analysis::taint::InferenceRulesBuilder;
using analysis::taint::TagId;

namespace {

analysis::taint::ModifyTag::Kind GetModifyTagKind(
    const SqlPolicyRule::Result::Action& action) {
  using raksha::analysis::taint::ModifyTag;
  using ActionProto = SqlPolicyRule::Result::Action;
  switch (action) {
    case ActionProto::SqlPolicyRule_Result_Action_ADD_CONFIDENTIALITY:
      return ModifyTag::Kind::kAddSecrecy;
    case ActionProto::SqlPolicyRule_Result_Action_REMOVE_CONFIDENTIALITY:
      return ModifyTag::Kind::kRemoveSecrecy;
    case ActionProto::SqlPolicyRule_Result_Action_ADD_INTEGRITY:
      return ModifyTag::Kind::kAddIntegrity;
    default:
      LOG(FATAL) << "Unknown category in SqlPolicyRule::Result::Action";
  }
}

void AddInferenceRuleForMerge(const sql::MergeOp& merge_op,
                              analysis::taint::MergeIntegrityTags::Kind kind,
                              absl::string_view action_name,
                              InferenceRulesBuilder& rules_builder) {
  std::vector<uint64_t> indices(merge_op.GetControlInputStartIndex());
  // Fill indices with 0, 1, ... merge_op.GetControlInputStartIndex() - 1
  absl::c_generate(indices, [n = 0]() mutable { return n++; });

  std::vector<InferenceRule> rules = {
      {.conclusion = analysis::taint::MergeIntegrityTags(
           {.kind = kind, .input_indices = std::move(indices)}),
       .premises = {}}};
  rules_builder.AddOutputRulesForAction(
      action_name, {{DecoderContext::kDefaultOutputIndex, rules}});
}

void AddInferenceRuleForTagTransform(const SqlPolicyRule& sql_policy_rule,
                                     const TagTransformOp& tag_transform_op,
                                     absl::string_view action_name,
                                     InferenceRulesBuilder& rules_builder) {
  std::vector<analysis::taint::InputHasIntegrity> premises;
  auto input_indices = tag_transform_op.GetPreconditionInputIndices();
  for (const auto& precondition : sql_policy_rule.preconditions()) {
    auto tag =
        rules_builder.GetOrCreateTagId(precondition.required_integrity_tag());
    // Find the index.
    auto index_iter = input_indices.find(precondition.argument());
    CHECK(index_iter != input_indices.end())
        << "Unable to find input index for '" << precondition.argument()
        << "'.";
    premises.push_back({.input_index = index_iter->second, .tag = tag});
  }

  InferenceRule copy_input_rule = {
      .conclusion = analysis::taint::CopyInput(
          {.input_index = tag_transform_op.GetTransformedValueIndex()}),
      .premises = {}};
  InferenceRule modify_tag_rule({
      .conclusion = analysis::taint::ModifyTag(
          {.kind = GetModifyTagKind(sql_policy_rule.result().action()),
           .tag =
               rules_builder.GetOrCreateTagId(sql_policy_rule.result().tag())}),
      .premises = std::move(premises),
  });
  rules_builder.AddOutputRulesForAction(
      action_name,
      {{0, {std::move(copy_input_rule), std::move(modify_tag_rule)}}});
}

}  // namespace

InferenceRules DecodeSqlPolicyRules(const ExpressionArena& expr,
                                    const ir::Module& module,
                                    std::vector<std::string> seed_tags) {
  InferenceRulesBuilder rules_builder(std::move(seed_tags));
  absl::flat_hash_map<std::string, const SqlPolicyRule*> sql_policy_rules;
  for (const SqlPolicyRule& sql_policy_rule : expr.sql_policy_rules()) {
    auto [_, inserted] =
        sql_policy_rules.insert({sql_policy_rule.name(), &sql_policy_rule});
    CHECK(inserted) << "Dupliate entry for rule name '"
                    << sql_policy_rule.name() << "'";
  }
  // In the current encoding of SqlPolicyRules, the rule_name does not uniquely
  // identify the behavior with respect to input arguments. Here are a couple of
  // examples:
  //    %3 = tag_transform [rule_name: "Rule1", arg_a: 1, arg_b: 2](%0, %1, %2)
  //    %7 = tag_transform [rule_name: "Rule1", arg_a: 2, arg_b: 1](%4, %5, %6)
  //
  // This map allows us to uniquify rule_names when we use it as actions.
  absl::flat_hash_map<std::string, uint64_t> rule_counts;
  for (const auto& block : module.blocks()) {
    for (const auto& operation : block->operations()) {
      const auto* tag_transform_op = SqlOp::GetIf<TagTransformOp>(*operation);
      if (tag_transform_op == nullptr) continue;

      absl::string_view tag_transform_name = tag_transform_op->GetRuleName();
      std::string action_name = absl::StrCat(tag_transform_name, "_",
                                             rule_counts[tag_transform_name]++);

      if (tag_transform_name == TagTransformOp::kIntersectItagRule ||
          tag_transform_name == TagTransformOp::kUnionItagRule) {
        CHECK(tag_transform_op->inputs().size() == 1)
            << "Special tag transform op has more than one input.";

        // Consider the following sequence:
        //     input = sql.merge [...](...)
        //     ...
        //     result = sql.tag_transform [rule_name:...](input)
        //
        // A tag transform with special intersect/union rule defines the
        // transformation for the first argument of tag transform (`input`).
        ir::Value input = tag_transform_op->GetTransformedValue();
        auto merge_result = input.If<ir::value::OperationResult>();
        CHECK(merge_result != nullptr)
            << "Argument of a special tag_transform_op "
               "is not an operation result.";
        rules_builder.MarkOperationAsAction(merge_result->operation(),
                                            action_name);
        const auto* merge_op = SqlOp::GetIf<MergeOp>(merge_result->operation());
        CHECK(merge_op != nullptr)
            << "Special tag_transform_op is not referring to a MergeOp";

        analysis::taint::MergeIntegrityTags::Kind kind =
            (tag_transform_name == TagTransformOp::kUnionItagRule)
                ? analysis::taint::MergeIntegrityTags::Kind::kUnion
                : analysis::taint::MergeIntegrityTags::Kind::kIntersect;
        AddInferenceRuleForMerge(*merge_op, kind, action_name, rules_builder);
      } else {
        // Mark this operation as the `action_name` action.
        rules_builder.MarkOperationAsAction(*operation, action_name);

        auto rule_iter = sql_policy_rules.find(tag_transform_name);
        CHECK(rule_iter != sql_policy_rules.end())
            << "No sql policy rule found for '" << tag_transform_name << "'.";
        const SqlPolicyRule& sql_policy_rule = *rule_iter->second;
        // Add the inference rules for this action.
        AddInferenceRuleForTagTransform(sql_policy_rule, *tag_transform_op,
                                        action_name, rules_builder);
      }
    }
  }
  return rules_builder.Build();
}

}  // namespace raksha::frontends::sql
