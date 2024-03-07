#include "src/common/testing/gtest.h"
#include "src/policy/parser.h"

/**
 * Policy for SQL Tables covering AmbientContext.
 *
 * This policy is a translation of the AmbientContext Chronicle policy for the
 * SQL layout of simple storage.
 *
 * @see [AmbientContextPolicy]
 * (http://google3/java/com/google/android/libraries/pcc/policies/federatedcompute/AmbientContextPolicy_FederatedCompute.kt)
 * Chronicle Policy :
 * https://source.corp.google.com/piper///depot/google3/java/com/google/android/apps/miphone/aiai/chronicle/data/storage/simplestorage/policy/AmbientContextSqlPolicy.kt
 */

namespace raksha::policy {

using testing::TestWithParam;
using testing::ValuesIn;

class PolicyParserTest
    : public TestWithParam<std::tuple<absl::string_view, bool>> {};

TEST_P(PolicyParserTest, SimpleTest) {
  auto [input_program_text, is_policy_valid] = GetParam();
  auto result = ParseProgramIsValid(input_program_text);
  EXPECT_EQ(result, is_policy_valid);
}

constexpr std::tuple<absl::string_view, bool> kTestStatements[] = {
    // sqlPolicy rules
    // EventTimestampsCanBeOrderedAndEgressedTruncatedToDays
    // EventTimestampMillis.isUnrestrictedAfter(
    //  BinaryExpression(Variable(""), NumericLiteral("86400000"), DIVIDE) ),
    // expected IR translation for above BinaryExpression
    // %div_result = core.div[](%x, %y)

    {R"(decl action "arithmetic " {}
op is action "arithmetic" if {
  op.operator:name = "core.div"
} with {
 args[0] := op.args[0],    
 args[1] := op.args[1]
}
after action "arithmetic" {
  remove "Restricted" from action.result[0] if {
    action.data[0]:Integrity = "EventTimeStampMillis",
    action.data[1]:constant  = 86400000
}
})",
     true},
    // comments for above rule : maybe action.data[0] makes it easy to
    // understand than args[0] in with clause

    // EventTimestampMillis.canBeUsedForOrdering()
    // Allows comparing values labelled with the receiver [IntegrityLabel] with
    // ordering operators: <,
    // <=, >, >=.
    // with pattern ?a < ?b, ?a <= ?b, ?a > ?b, ?a >= ?b, remove "Restricted"
    // confidentiality label if both a and b have EventTimeStampMillis integrity
    // label

    {R"(decl action "comparison" {}
    op is action "comparison" if {
      op.operator: name = "core.less_than" }
    with {
      args[0] := op.args[0],
      args[1] := op.args[1]
    }
    op is action "comparison" if {
      op.operator: name = "core.less_than_equal" }
    with{
      args[0] := op.args[0],
      args[1] := op.args[1]
    }
    op is action "comparison" if {
      op.operator: name = "core.greater_than" }
    with{
      args[0] := op.args[0],
      args[1] := op.args[1]
    }
    op is action "comparison" if {
      op.operator: name = "core.greater_than_equal" }
    with{
      args[0] := op.args[0],
      args[1] := op.args[1]
    }
    after action "comparison" {
      remove "Restricted" from action.result[0] if {
      action.data[0] : Integrity = "EventTimeStampMillis",
      action.data[1] : Integrity = "EventTimeStampMillis"
    }
    }
    )",
     true},
    // EventTimestampCanJoinWithinTimeWindows_UpTo3d
    // Patterns:
    //   ?timestamp1 + ?window > ?timestamp2
    //   ?timestamp1 + ?window >= ?timestamp2
    //   ?timestamp1 + ?window < ?timestamp2
    //   ?timestamp1 + ?window <= ?timestamp2
    //   ?timestamp1 - ?window > ?timestamp2
    //   ?timestamp1 - ?window >= ?timestamp2
    //   ?timestamp1 - ?window < ?timestamp2
    //   ?timestamp1 - ?window <= ?timestamp2
    //   ?timestamp1 > ?timestamp2 + ?window
    //   ?timestamp1 >= ?timestamp2 + ?window
    //   ?timestamp1 < ?timestamp2 + ?window
    //   ?timestamp1 <= ?timestamp2 + ?window
    //   ?timestamp1 > ?timestamp2 - ?window
    //   ?timestamp1 >= ?timestamp2 - ?window
    //   ?timestamp1 < ?timestamp2 - ?window
    //   ?timestamp1 <= ?timestamp2 - ?window
    // %result = "core.JoinWithInWindow[](%timestamp1, %window, %timestamp2)"
    // arithmetic and comparison op is action rules defined above
    {R"(decl action "arithmetic" {}
    decl action "comparision" {}
    after action "arithmetic" {
      add "TimeStamp1" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp1"
      }
      add "TimeStamp2" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp2"
      }
      add "AllowedTimeWindowMillis" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp2",
        action.data[1]: constant = 1000 
      }
      add "AllowedTimeWindowMillis" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp2",
        action.data[1]: constant = 5000
      }
      add "AllowedTimeWindowMillis" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp2",
        action.data[1]: constant = 15000
      }
    }
    after action "comparision" {
      remove "Restricted" from action.result[0] if {
        action.data[0]: Integrity = "TimeStamp1",
        action.data[0]:Integrity = "AllowedTimeWindowMillis",
        action.data[1]: Integrity = "TimeStamp2"
      }
      remove "Restricted" from action.result[0] if {
        action.data[0]: Integrity = "TimeStamp2",
        action.data[0]:Integrity = "AllowedTimeWindowMillis",
        action.data[1]: Integrity = "TimeStamp1"
      }
    })",
     true},
    // comments for above instead of repeating add "AllowedTimeWindowMillis"
    // rule for each of constant, grammar should add an 'or' for attribute
    // values similar to below
    /* add "AllowedTimeWindowMillis" to action.result[0] if {
        action.data[0] : Integrity = "TimeStamp2",
        action.data[1]: constant = 1000 | 5000 | 15000 | 6000 | 30000 | 900000
                          | 36000000 | 10800000 | 21600000 | 43200000
                          | 86400000 | 172800000 | 259200000
      }*/

    // SystemInfoPolicy
    // SystemInfoId.canBeEqualityCompared()
    {R"(decl action "equality_comparison" {}
    op is action "equality_comparison" if {
      op.operator: name = "core.equal" }
    with{
      args[0] := op.args[0],
      args[1] := op.args[1]
    }
    op is action "equality_comparison" if {
        op.operator: name = "core.not_equal" }
    with{
          args[0] := op.args[0],
          args[1] := op.args[1]
        }
    after action "equality_comparison" {
      remove "Restricted" from action.result[0] if {
          action.data[0] : Integrity = "SystemInfoId",
          action.data[1] : Integrity = "SystemInfoId"
        }})",
     true},
     
    // AndroidPackageNamePolicy
    // AndroidPackageName.canBeEqualityCompared(), i.e with pattern ?a == ?b, ?a
    // != ?b and ('a' and 'b') both have Integrity label "AndroidPackageName",
    // then
    // remove "Restricted" confidentiality label on the result
    {R"(decl action "equality_comparison" {}
    after action "equality_comparison" {
      remove "Restricted" from action.result[0] if {
          action.data[0] : Integrity = "AndroidPackageName",
          action.data[1] : Integrity = "AndroidPackageName"
        }})",
     true},
    // sqlPolicy Tables is a map of TableIdentifier( A datastructure with
    // SchemaName and tableName) and table policy (map of clumn names and
    // set of
    // confidentiality and integrity labels associated with the columns).A
    // Raksha IR has stores associated for every column in a table. We use
    // addCategory raksha policy rule to add labels to stores in following
    // way
    {R"(add "Restricted" to "AmbientContextStatus.timestampMillis")", true},
    {R"(add "EventTimeStampMillis" to "AmbientContextStatus.timestampMillis")",
     true},
    {R"(add "Restricted" to "AmbientContextStatus.packageName")", true},
    {R"(add "AndroidPackageName" to "AmbientContextStatus.packageName")", true},

};

INSTANTIATE_TEST_SUITE_P(PolicyParserTest, PolicyParserTest,
                         ValuesIn(kTestStatements));
}  // namespace raksha::policy
