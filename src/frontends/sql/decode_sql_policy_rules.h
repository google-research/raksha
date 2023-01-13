#ifndef SRC_FRONTENDS_SQL_DECODE_SQL_POLICY_RULES_H_
#define SRC_FRONTENDS_SQL_DECODE_SQL_POLICY_RULES_H_

#include "src/analysis/taint/inference_rules.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/ir/module.h"

namespace raksha::frontends::sql {

// Decodes and returns the SqlPolicyRules in `ExpressionArena` protobuf as
// inference rules for taint analysis. The seed_tags is used to assign
// predictable ids for tag names if needed.
analysis::taint::InferenceRules DecodeSqlPolicyRules(
    const ExpressionArena& expr, const ir::Module& module,
    std::vector<std::string> seed_tags = {});

}  // namespace raksha::frontends::sql
#endif  // SRC_FRONTENDS_SQL_DECODE_SQL_POLICY_RULES_H_
