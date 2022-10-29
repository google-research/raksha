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
// WITHOUT WRRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/backends/policy_engine/chaotic/chaotic_policy_checker.h"

#include <cstddef>
#include <iterator>
#include <optional>

#include "absl/algorithm/container.h"
#include "absl/strings/match.h"
#include "absl/strings/str_join.h"
#include "src/backends/policy_engine/chaotic/sql_policy_rules.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/frontends/sql/ops/sql_output_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ir_traversing_visitor.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine {

using raksha::frontends::sql::MergeOp;
using raksha::frontends::sql::SqlOp;
using raksha::frontends::sql::SqlOutputOp;
using raksha::frontends::sql::TagTransformOp;

class ChaoticIterator {
 public:
  ChaoticIterator(const ir::Module& module, const SqlPolicyRules& rules);

  bool compliance() const { return compliance_; }

 private:
  bool ComputeFixpoint(const ir::Module& module, const SqlPolicyRules& rules);

  const SqlPolicyRules::TagSet* GetIntegrityTags(const ir::Value& value) const {
    auto find_result = integrity_tags_.find(value);
    return find_result != integrity_tags_.end() ? &find_result->second
                                                : nullptr;
  }

  const SqlPolicyRules::TagSet* GetSecrecyTags(const ir::Value& value) const {
    auto find_result = secrecy_tags_.find(value);
    return find_result != secrecy_tags_.end() ? &find_result->second : nullptr;
  }

  bool compliance_;
  absl::flat_hash_map<ir::Value, SqlPolicyRules::TagSet> integrity_tags_;
  absl::flat_hash_map<ir::Value, SqlPolicyRules::TagSet> secrecy_tags_;
};

ChaoticIterator::ChaoticIterator(const ir::Module& module,
                                 const SqlPolicyRules& rules) {
  compliance_ = ComputeFixpoint(module, rules);
}

bool ChaoticIterator::ComputeFixpoint(const ir::Module& module,
                                      const SqlPolicyRules& rules) {
  absl::flat_hash_set<const ir::Operation*> worklist;
  for (const auto& block : module.blocks()) {
    for (const auto& operation : block->operations()) {
      worklist.insert(operation.get());
    }
  }
  bool changed = true;
  while (changed) {
    changed = false;
    for (const ir::Operation* op : worklist) {
      ir::Value result = ir::Value::MakeOperationResultValue(*op, 0);
      auto result_integrity = integrity_tags_.insert({result, {}});
      if (result_integrity.second) {
        changed = true;
      }
      auto result_secrecy = secrecy_tags_.insert({result, {}});
      if (result_secrecy.second) {
        changed = true;
      }
      SqlPolicyRules::TagSet result_secrecy_new;
      SqlPolicyRules::TagSet result_integrity_new;
      const SqlOp* sql_op = CHECK_NOTNULL(dynamic_cast<const SqlOp*>(op));
      if (const SqlOp* merge_op = SqlOp::GetIf<MergeOp>(*sql_op)) {
        for (const auto& input : merge_op->inputs()) {
          const auto* input_secrecy = GetSecrecyTags(input);
          if (input_secrecy == nullptr) continue;
          result_secrecy_new.insert(input_secrecy->begin(),
                                    input_secrecy->end());
        }
      } else if (const TagTransformOp* tag_transform_op =
                     SqlOp::GetIf<TagTransformOp>(*op)) {
        ir::Value value = tag_transform_op->GetTransformedValue();
        const auto* input_secrecy = GetSecrecyTags(value);
        if (input_secrecy != nullptr) {
          result_secrecy_new.insert(input_secrecy->begin(),
                                    input_secrecy->end());
        }

        absl::string_view rule_name = tag_transform_op->GetRuleName();
        if (rule_name != "##raksha_internal#intersect_itag_rule" &&
            rule_name != "##raksha_internal#union_itag_rule") {
          const SqlPolicyRule* rule = rules.GetRule(rule_name);
          LOG_IF(FATAL, rule == nullptr) << "Cannot find rule " << rule_name;
          const auto* input_integrity = GetIntegrityTags(value);
          if (input_integrity != nullptr) {
            result_integrity_new.insert(input_integrity->begin(),
                                        input_integrity->end());
          }
          bool rule_fires = true;
          for (const auto& [name, precond_value] :
               tag_transform_op->GetPreconditions()) {
            const auto* precondition_integrity =
                GetIntegrityTags(precond_value);
            if (precondition_integrity == nullptr) {
              rule_fires = false;
              break;
            }
            auto tag_set = rule->GetPreconditionTagSet(name);
            if (tag_set == nullptr) {
              rule_fires = false;
              break;
            }
            if (!absl::c_all_of(*tag_set, [precondition_integrity](
                                              const SqlPolicyRules::Tag& tag) {
                  return precondition_integrity->contains(tag);
                })) {
              rule_fires = false;
              break;
            }
          }
          if (rule_fires) {
            switch (rule->result().action()) {
              case SqlPolicyRule::Result::Action::AddConfidentialityTag:
                result_secrecy_new.insert(rule->result().tag());
                break;
              case SqlPolicyRule::Result::Action::RemoveConfidentialityTag:
                result_secrecy_new.erase(rule->result().tag());
                break;
              case SqlPolicyRule::Result::Action::AddIntegrityTag:
                result_integrity_new.insert(rule->result().tag());
                break;
              default:
                CHECK(false);
            }
          }
        }
      }
      if (result_integrity_new != result_integrity.first->second) {
        changed = true;
        integrity_tags_.insert_or_assign(result, result_integrity_new);
      }
      if (result_secrecy_new != result_secrecy.first->second) {
        changed = true;
        secrecy_tags_.insert_or_assign(result, result_secrecy_new);
      }
    }
  }
  // ir::SsaNames ssa_names;
  // LOG(ERROR) << " Module is " << ir::IRPrinter::ToString(module, ssa_names);
  // LOG(ERROR) << "Secrecy Tags:";
  // for (const auto& [value, tags] : secrecy_tags_) {
  //   LOG(ERROR) << value.ToString(ssa_names) << " : {"
  //              << absl::StrJoin(
  //                     tags, ",",
  //                     [&rules](std::string* out, SqlPolicyRules::Tag tag) {
  //                       auto tag_name = rules.GetTagName(tag);
  //                       absl::StrAppend(
  //                           out, tag_name.has_value() ? *tag_name :
  //                           "UNKNOWN");
  //                     })
  //              << "}";
  // }
  // LOG(ERROR) << "Integrity Tags:";
  // for (const auto& [value, tags] : integrity_tags_) {
  //   LOG(ERROR) << value.ToString(ssa_names) << " : {"
  //              << absl::StrJoin(
  //                     tags, ",",
  //                     [&rules](std::string* out, SqlPolicyRules::Tag tag) {
  //                       auto tag_name = rules.GetTagName(tag);
  //                       absl::StrAppend(
  //                           out, tag_name.has_value() ? *tag_name :
  //                           "UNKNOWN");
  //                     })
  //              << "}";
  // }
  bool compliance = true;
  for (const ir::Operation* op : worklist) {
    const SqlOp* sql_op = dynamic_cast<const SqlOp*>(op);
    if (sql_op == nullptr) continue;
    const SqlOutputOp* sql_output_op = SqlOp::GetIf<SqlOutputOp>(*op);
    if (sql_output_op == nullptr) continue;
    for (const auto& input : sql_output_op->inputs()) {
      const auto* secrecy_tags = GetSecrecyTags(input);
      if (secrecy_tags == nullptr) continue;
      if (!secrecy_tags->empty()) {
        compliance = false;
        break;
      }
    }
  }
  return compliance;
}

bool ChaoticPolicyChecker::IsModulePolicyCompliant(const ir::Module& module,
                                                   const Policy& policy) const {
  ChaoticIterator iterator(module, policy_rules_);
  return iterator.compliance();
}

}  // namespace raksha::backends::policy_engine
