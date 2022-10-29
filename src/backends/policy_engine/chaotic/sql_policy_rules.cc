#include "src/backends/policy_engine/chaotic/sql_policy_rules.h"

#include <iostream>
#include <sstream>

#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "src/common/utils/map_iter.h"
#include "src/frontends/sql/sql_ir.proto.h"

namespace raksha::backends::policy_engine {

SqlPolicyRules::SqlPolicyRules(const frontends::sql::ExpressionArena& arena) {
  for (const frontends::sql::SqlPolicyRule& rule : arena.sql_policy_rules()) {
    absl::flat_hash_map<std::string, SqlPolicyRule::TagSet> preconditions;
    for (const auto& precondition_proto : rule.preconditions()) {
      auto tag = GetOrCreateTag(precondition_proto.required_tag());
      preconditions[precondition_proto.argument()].insert(tag);
    }
    auto action = SqlPolicyRule::Result::Action::AddConfidentialityTag;
    switch (rule.result().action()) {
      case frontends::sql::SqlPolicyRule::Result::Action::
          SqlPolicyRule_Result_Action_ADD_CONFIDENTIALITY:
        action = SqlPolicyRule::Result::Action::AddConfidentialityTag;
        break;
      case frontends::sql::SqlPolicyRule::Result::Action::
          SqlPolicyRule_Result_Action_REMOVE_CONFIDENTIALITY:
        action = SqlPolicyRule::Result::Action::RemoveConfidentialityTag;
        break;
      case frontends::sql::SqlPolicyRule::Result::Action::
          SqlPolicyRule_Result_Action_ADD_INTEGRITY:
        action = SqlPolicyRule::Result::Action::AddIntegrityTag;
        break;
      default:
        action = SqlPolicyRule::Result::Action::AddConfidentialityTag;
    }

    rules_.insert(
        {rule.name(),
         SqlPolicyRule(
             rule.name(),
             SqlPolicyRule::Result(action, GetOrCreateTag(rule.result().tag())),
             preconditions)});
  }
}

}  // namespace raksha::backends::policy_engine
