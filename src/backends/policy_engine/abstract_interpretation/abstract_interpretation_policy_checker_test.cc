#include "src/backends/policy_engine/abstract_interpretation/abstract_interpretation_policy_checker.h"

#include <utility>

#include "google/protobuf/text_format.h"
#include "absl/container/flat_hash_map.h"
#include "src/analysis/taint/abstract_ifc_tags.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/backends/policy_engine/sql_policy_rule_policy.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/ranges.h"
#include "src/frontends/sql/decode_sql_policy_rules.h"
#include "src/frontends/sql/ops/op_traits.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/ir/value_string_converter.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::backends::policy_engine {

namespace {

using analysis::taint::AbstractIfcTags;
using analysis::taint::InferenceRules;
using analysis::taint::TagId;
using analysis::taint::TagIdSet;
using frontends::sql::OpTraits;
using frontends::sql::SqlOutputOp;
using parser::ir::IrProgramParserResult;
using ::testing::Eq;
using ::testing::UnorderedPointwise;
using ::testing::ValuesIn;

const std::vector<std::string> kTagNames = {"A", "B", "C", "D", "E"};
constexpr TagId kTagA = 0;
constexpr TagId kTagB = 1;
constexpr TagId kTagC = 2;

struct TestCase {
  std::string test_name;
  std::string module_text;
  bool policy_compliant;
  absl::flat_hash_map<std::string, AbstractIfcTags> fixpoint_result;
};

class AbstractInterpretationPolicyCheckerTest
    : public ::testing::TestWithParam<TestCase> {
 public:
  AbstractInterpretationPolicyCheckerTest()
      : parse_result_(parser::ir::ParseProgram(GetParam().module_text)),
        inference_rules_(BuildInferenceRules(module())) {}

  const ir::Module& module() const {
    return *ABSL_DIE_IF_NULL(parse_result_.module);
  }
  ir::SsaNames* ssa_names() const {
    return ABSL_DIE_IF_NULL(parse_result_.ssa_names.get());
  }
  const InferenceRules& inference_rules() const { return inference_rules_; }

  static InferenceRules BuildInferenceRules(const ir::Module& module) {
    frontends::sql::ExpressionArena expr;
    bool parse_success = google::protobuf::TextFormat::Parser().ParseFromString(
        std::string(kRules), &expr);
    CHECK(parse_success) << "Unable to parse proto text for ExpressionArena:\n "
                         << kRules;
    return frontends::sql::DecodeSqlPolicyRules(expr, module, kTagNames);
  }

 private:
  static constexpr absl::string_view kRules = R"(
  sql_policy_rules {
    name: "AddSecrecyA"
    result {
      action: ADD_CONFIDENTIALITY
      tag: "A"
    }
  }
  sql_policy_rules {
    name: "AddSecrecyB"
    result {
      action: ADD_CONFIDENTIALITY
      tag: "B"
    }
  }
  sql_policy_rules {
    name: "AddSecrecyC"
    result {
      action: ADD_CONFIDENTIALITY
      tag: "C"
    }
  }
  sql_policy_rules {
    name: "RemoveSecrecyC"
    result {
      action: REMOVE_CONFIDENTIALITY
      tag: "C"
    }
  }
  sql_policy_rules {
    name: "AddIntegrityA"
    result {
      action: ADD_INTEGRITY
      tag: "A"
    }
  }
  sql_policy_rules {
    name: "AddIntegrityB"
    result {
      action: ADD_INTEGRITY
      tag: "B"
    }
  }
  sql_policy_rules {
    name: "ConditionalAddSecrecyC"
    result {
      action: ADD_CONFIDENTIALITY
      tag: "C"
    }
    preconditions {
      argument: "arg_a"
      required_integrity_tag: "A"
    }
    preconditions {
      argument: "arg_b"
      required_integrity_tag: "B"
    }
  }
  sql_policy_rules {
    name: "ConditionalRemoveSecrecyC"
    result {
      action: REMOVE_CONFIDENTIALITY
      tag: "C"
    }
    preconditions {
      argument: "arg_a"
      required_integrity_tag: "A"
    }
    preconditions {
      argument: "arg_b"
      required_integrity_tag: "B"
    }
  }
  sql_policy_rules {
    name: "ConditionalAddIntegrityB"
    result {
      action: ADD_INTEGRITY
      tag: "B"
    }
    preconditions {
      argument: "arg_a"
      required_integrity_tag: "A"
    }
  })";

  IrProgramParserResult parse_result_;
  InferenceRules inference_rules_;
};

MATCHER_P(EquivalentToValueForSsaNameKey, ssa_names, "") {
  const auto& [actual, expected] = arg;
  const auto& [actual_value, actual_tags] = actual;
  const auto& [expected_value_name, expected_tags] = expected;
  std::string actual_value_name = ir::ValueToString(actual_value, *ssa_names);
  return (actual_value_name == expected_value_name &&
          actual_tags.IsEquivalentTo(expected_tags));
}

TEST_P(AbstractInterpretationPolicyCheckerTest, ExpectedFixpointIsComputed) {
  AbstractInterpretationPolicyChecker checker(inference_rules(), {});
  TaintAnalysis::ValueStateMap results = checker.ComputeIfcTags(module());
  EXPECT_THAT(results,
              UnorderedPointwise(EquivalentToValueForSsaNameKey(ssa_names()),
                                 GetParam().fixpoint_result));
}

TEST_P(AbstractInterpretationPolicyCheckerTest, PolicyViolationCheckIsCorrect) {
  AbstractInterpretationPolicyChecker checker(
      inference_rules(), {std::string(OpTraits<SqlOutputOp>::kName)});
  SqlPolicyRulePolicy policy("");
  EXPECT_THAT(checker.IsModulePolicyCompliant(module(), policy),
              Eq(GetParam().policy_compliant));
}

TEST(AbstractInterpretationPolicyCheckerTest,
     PolicyViolationCheckRespectsEgress) {
  constexpr absl::string_view module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddSecrecyA"](%0)
    %2 = sql.tag_transform [rule_name: "AddSecrecyB"](%0)
    %3 = sql.sql_output [] (%1)
    %4 = sql.screen_output [] (%2)
    %5 = safe_output[] (%0)
  }
})";
  IrProgramParserResult parse_result = parser::ir::ParseProgram(module_text);
  const ir::Module& module = *parse_result.module;
  InferenceRules inference_rules =
      AbstractInterpretationPolicyCheckerTest::BuildInferenceRules(module);
  SqlPolicyRulePolicy policy("");

  EXPECT_THAT(AbstractInterpretationPolicyChecker(
                  inference_rules, {"sql.sql_output", "sql.screen_output"})
                  .IsModulePolicyCompliant(module, policy),
              Eq(false));
  EXPECT_THAT(
      AbstractInterpretationPolicyChecker(inference_rules, {"sql.sql_output"})
          .IsModulePolicyCompliant(module, policy),
      Eq(false));
  EXPECT_THAT(AbstractInterpretationPolicyChecker(inference_rules,
                                                  {"sql.screen_output"})
                  .IsModulePolicyCompliant(module, policy),
              Eq(false));
  EXPECT_THAT(
      AbstractInterpretationPolicyChecker(inference_rules, {"safe_output"})
          .IsModulePolicyCompliant(module, policy),
      Eq(true));
}

const TestCase kTestCases[] = {
    {.test_name = "StraightAddSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %2 = sql.sql_output [] (%1)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({kTagC}, {})},
                         {"%2", AbstractIfcTags({kTagC}, {})}}},
    {.test_name = "StraightLineAddIntegrity",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %2 = sql.sql_output [] (%1)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagB})},
                         {"%2", AbstractIfcTags({}, {})}}},
    {.test_name = "StraightLineAddRemoveSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %2 = sql.tag_transform [rule_name: "RemoveSecrecyC"](%1)
    %3 = sql.sql_output [] (%2)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({kTagC}, {})},
                         {"%2", AbstractIfcTags({}, {})},
                         {"%3", AbstractIfcTags({}, {})}}},
    {.test_name = "StraightLineConditionalRemoveSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %1, %2)
    %5 = sql.sql_output [] (%4)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({}, {})},
                         {"%5", AbstractIfcTags({}, {})}}},
    {.test_name = "StraightLineConditionalRemoveSecrecyFail",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    // Attributes arg_a and arg_b are wrong and prevents "C" from being removed.
    %4 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 2, arg_b: 1](%3, %1, %2)
    %5 = sql.sql_output [] (%4)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({kTagC}, {})},
                         {"%5", AbstractIfcTags({kTagC}, {})}}},
    {.test_name = "StraightLineConditionalAddIntegrity",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "ConditionalAddIntegrityB", 
                            arg_a: 1](%0, %1)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %1, %2)
    %5 = sql.sql_output [] (%4)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({}, {})},
                         {"%5", AbstractIfcTags({}, {})}}},
    {.test_name = "StraightLineConditionalAddIntegrityFail",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "ConditionalAddIntegrityB", 
                            arg_a: 1](%0, %0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %1, %2)
    %5 = sql.sql_output [] (%4)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({kTagC}, {})},
                         {"%5", AbstractIfcTags({kTagC}, {})}}},
    {.test_name = "MergeAddSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddSecrecyA"](%0)
    %2 = sql.tag_transform [rule_name: "AddSecrecyB"](%1)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.merge [control_input_start_index: 1](%1, %3)
    %5 = sql.merge [control_input_start_index: 2](%2, %3)
    %6 = sql.sql_output [] (%5)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({kTagA}, {})},
                         {"%2", AbstractIfcTags({kTagA, kTagB}, {})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({kTagA, kTagC}, {})},
                         {"%5", AbstractIfcTags({kTagA, kTagB, kTagC}, {})},
                         {"%6", AbstractIfcTags({kTagA, kTagB, kTagC}, {})}}},

    {.test_name = "MergeConditionalRemoveSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.merge [control_input_start_index: 2](%1, %2)
    %5 = sql.tag_transform [rule_name: "##raksha_internal#union_itag_rule"](%4)
    %6 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %4, %4)
    %7 = sql.sql_output [] (%6)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({}, {kTagA, kTagB})},
                         {"%5", AbstractIfcTags({}, {})},
                         {"%6", AbstractIfcTags({}, {})},
                         {"%7", AbstractIfcTags({}, {})}}},
    {.test_name = "MergeConditionalRemoveSecrecyFail01",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %4 = sql.merge [control_input_start_index: 2](%1, %2)
    // Intersecting instead of unioning the integrity tags.
    %5 = sql.tag_transform
      [rule_name: "##raksha_internal#intersect_itag_rule"](%4)
    %6 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %4, %4)
    %7 = sql.sql_output [] (%6)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({}, {})},
                         {"%5", AbstractIfcTags({}, {})},
                         {"%6", AbstractIfcTags({kTagC}, {})},
                         {"%7", AbstractIfcTags({kTagC}, {})}}},
    {.test_name = "MergeConditionalRemoveSecrecyFail02",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %2 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %3 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    // Wrong control start index.
    %4 = sql.merge [control_input_start_index: 1](%1, %2)
    %5 = sql.tag_transform
      [rule_name: "##raksha_internal#union_itag_rule"](%4)
    %6 = sql.tag_transform [rule_name: "ConditionalRemoveSecrecyC",
                            arg_a: 1, arg_b: 2](%3, %4, %4)
    %7 = sql.sql_output [] (%6)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagA})},
                         {"%2", AbstractIfcTags({}, {kTagB})},
                         {"%3", AbstractIfcTags({kTagC}, {})},
                         {"%4", AbstractIfcTags({}, {kTagA})},
                         {"%5", AbstractIfcTags({}, {})},
                         {"%6", AbstractIfcTags({kTagC}, {})},
                         {"%7", AbstractIfcTags({kTagC}, {})}}},
    {.test_name = "CycleAddIntegrity",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddIntegrityB"](%0)
    %2 = sql.merge [control_input_start_index: 2](%1, %5)
    %3 = sql.tag_transform
      [rule_name: "##raksha_internal#union_itag_rule"](%2)
    %4 = sql.tag_transform [rule_name: "AddIntegrityA"](%0)
    %5 = sql.merge [control_input_start_index: 1](%4)
    %6 = sql.tag_transform
      [rule_name: "##raksha_internal#union_itag_rule"](%5)
    %7 = sql.sql_output [](%2)
  }
})",
     .policy_compliant = true,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({}, {kTagB})},
                         {"%2", AbstractIfcTags({}, {kTagB, kTagA})},
                         {"%3", AbstractIfcTags({}, {})},
                         {"%4", AbstractIfcTags({}, {kTagA})},
                         {"%5", AbstractIfcTags({}, {kTagA})},
                         {"%6", AbstractIfcTags({}, {})},
                         {"%7", AbstractIfcTags({}, {})}}},
    {.test_name = "CycleAddSecrecy",
     .module_text = R"(
module m0 {
  block b0 {
    %0 = core.constant[] ()
    %1 = sql.tag_transform [rule_name: "AddSecrecyA"](%0)
    %2 = sql.merge [](%1, %5)
    %3 = sql.tag_transform [rule_name: "AddSecrecyB"](%0)
    %4 = sql.tag_transform [rule_name: "AddSecrecyC"](%0)
    %5 = sql.merge [control_input_start_index: 1](%3, %4)
    %6 = sql.sql_output [](%2)
  }
})",
     .policy_compliant = false,
     .fixpoint_result = {{"%0", AbstractIfcTags({}, {})},
                         {"%1", AbstractIfcTags({kTagA}, {})},
                         {"%2", AbstractIfcTags({kTagA, kTagB, kTagC}, {})},
                         {"%3", AbstractIfcTags({kTagB}, {})},
                         {"%4", AbstractIfcTags({kTagC}, {})},
                         {"%5", AbstractIfcTags({kTagB, kTagC}, {})},
                         {"%6", AbstractIfcTags({kTagA, kTagB, kTagC}, {})}}}};

INSTANTIATE_TEST_SUITE_P(
    AbstractInterpretationPolicyCheckerTests,
    AbstractInterpretationPolicyCheckerTest, ValuesIn(kTestCases),
    [](const testing::TestParamInfo<
        AbstractInterpretationPolicyCheckerTest::ParamType>& info) {
      return info.param.test_name;
    });

}  // namespace
}  // namespace raksha::backends::policy_engine
