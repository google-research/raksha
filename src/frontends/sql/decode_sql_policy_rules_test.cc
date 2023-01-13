#include "src/frontends/sql/decode_sql_policy_rules.h"

#include <cstdint>

#include "google/protobuf/text_format.h"
#include "absl/log/die_if_null.h"
#include "absl/strings/string_view.h"
#include "absl/strings/substitute.h"
#include "src/analysis/taint/inference_rule.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/ranges.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/ops/sql_op.h"
#include "src/frontends/sql/ops/tag_transform_op.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/ir/module.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::frontends::sql {
namespace {

using analysis::taint::Conclusion;
using analysis::taint::CopyInput;
using analysis::taint::InferenceRules;
using analysis::taint::InputHasIntegrity;
using analysis::taint::MergeIntegrityTags;
using analysis::taint::ModifyTag;
using analysis::taint::TagId;
using parser::ir::IrProgramParserResult;
using ::testing::Combine;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using ::testing::ValuesIn;

using ActionRules = analysis::taint::InferenceRules::ActionRules;
using InferenceRuleSequence =
    analysis::taint::InferenceRules::InferenceRuleSequence;
using OutputRules = analysis::taint::InferenceRules::OutputRules;

ExpressionArena ParseSqlPolicyRulesProtoText(absl::string_view text) {
  ExpressionArena result;
  bool parse_success =
      google::protobuf::TextFormat::Parser().ParseFromString(std::string(text), &result);
  CHECK(parse_success) << "Unable to parse proto text for ExpressionArena: "
                       << text;
  return result;
}

// Returns the operations of the given type `T` in the given module.
template <typename T>
std::vector<const T*> FindAllOperations(const ir::Module& module) {
  std::vector<const T*> result;
  for (const auto& block : module.blocks()) {
    for (const auto& operation : block->operations()) {
      if (const auto* op = SqlOp::GetIf<T>(*operation)) {
        result.push_back(op);
      }
    }
  }
  return result;
}

// Returns the single operation of given type `T` in the given module. Fails if
// there is more than one operation of type T in the given module.
template <typename T>
const T* FindOperation(const ir::Module& module) {
  std::vector<const T*> operations = FindAllOperations<T>(module);
  CHECK(operations.size() == 1) << "More than one operation of type "
                                << OpTraits<T>::kName << " in module.";
  return operations[0];
}

struct DecodeSqlPolicyRuleTestCase {
  std::string test_name;
  std::string proto_text;
  InferenceRuleSequence inference_rule_sequence;
};

// Update kTag... constants if you update kTagNames.
const std::vector<std::string> kTagNames = {"A", "B", "C"};
constexpr TagId kTagA = 0;
constexpr TagId kTagB = 1;
constexpr TagId kTagC = 2;

// An example that map from a SqlPolicyResult proto text to expected Conclusion.
struct SqlPolicyResultExample {
  std::string proto_text;
  Conclusion conclusion;
};

const SqlPolicyResultExample kSqlPolicyResultExamples[] = {
    {.proto_text = R"(
      result {
        action: ADD_CONFIDENTIALITY
        tag: "A"
      }
)",
     .conclusion =
         ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy, .tag = kTagA}},
    {.proto_text = R"(
      result {
        action: REMOVE_CONFIDENTIALITY
        tag: "B"
      }
)",
     .conclusion =
         ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy, .tag = kTagB}},
    {.proto_text = R"(
      result {
        action: ADD_INTEGRITY
        tag: "C"
      }
)",
     .conclusion =
         ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity, .tag = kTagC}},
};

struct SqlPolicyRulePreconditionExample {
  // Proto text of the precondition.
  std::string proto_text;
  // Mapping from argument name to the input index.
  std::string argument_attributes;
  // Result of the decoding the preconditions.
  std::vector<InputHasIntegrity> premises;
};

const SqlPolicyRulePreconditionExample kSqlPolicyRulePreconditionExamples[] = {
    {.proto_text = R"(
      preconditions {
        argument: "arg_a"
        required_integrity_tag: "A"
      }
    )",
     .argument_attributes = "arg_a: 1",
     .premises = {InputHasIntegrity{.input_index = 1, .tag = kTagA}}},
    {.proto_text = R"(
      preconditions {
        argument: "arg_b"
        required_integrity_tag: "B"
      }
    )",
     .argument_attributes = "arg_b: 2",
     .premises = {InputHasIntegrity{.input_index = 2, .tag = kTagB}}},
    {.proto_text = R"(
      preconditions {
        argument: "arg_a"
        required_integrity_tag: "A"
      }
      preconditions {
        argument: "arg_b"
        required_integrity_tag: "B"
      }
    )",
     .argument_attributes = "arg_a: 1, arg_b: 2",
     .premises = {InputHasIntegrity{.input_index = 1, .tag = kTagA},
                  InputHasIntegrity{.input_index = 2, .tag = kTagB}}},
    {.proto_text = R"(
      preconditions {
        argument: "arg_a"
        required_integrity_tag: "A"
      }
      preconditions {
        argument: "arg_b"
        required_integrity_tag: "B"
      }
      preconditions {
        argument: "arg_c"
        required_integrity_tag: "C"
      }
    )",
     .argument_attributes = "arg_a: 1, arg_b: 3, arg_c: 2",
     .premises = {InputHasIntegrity{.input_index = 1, .tag = kTagA},
                  InputHasIntegrity{.input_index = 3, .tag = kTagB},
                  InputHasIntegrity{.input_index = 2, .tag = kTagC}}}};

const std::vector<std::string> kRuleNames = {"Rule0", "Rule1", "Rule2"};

class DecodeSqlTagTransformOpRuleTest
    : public ::testing::TestWithParam<
          std::tuple<std::string, SqlPolicyResultExample,
                     SqlPolicyRulePreconditionExample>> {
 protected:
  // Constructs a sql policy rules proto text and returns the parsed proto.
  static ExpressionArena GetExpressionArena(const ParamType& param) {
    const auto& [rule_name, result, precondition] = param;
    std::string sql_policy_proto_text =
        absl::Substitute(R"(
    sql_policy_rules {
      name: "$0"
      $1
      $2
    }
  )",
                         rule_name, result.proto_text, precondition.proto_text);
    return ParseSqlPolicyRulesProtoText(sql_policy_proto_text);
  }

  // Constructs an example module text, parses it, and returns the parse result.
  static IrProgramParserResult GetParsedModule(const ParamType& param) {
    const auto& [rule_name, result, precondition] = param;
    std::string module_text =
        absl::Substitute(R"(
    module m0 {
      block b0 {
        %0, %1, %2, %3, %4 = core.multi_constant[] ()
        %5 = sql.tag_transform [rule_name: "$0", $1] (%0, %1, %2, %3, %4)
      }
    }
  )",
                         rule_name, precondition.argument_attributes);
    return parser::ir::ParseProgram(module_text);
  }
};

INSTANTIATE_TEST_SUITE_P(DecodeSqlTagTransformOpRuleTests,
                         DecodeSqlTagTransformOpRuleTest,
                         Combine(ValuesIn(kRuleNames),
                                 ValuesIn(kSqlPolicyResultExamples),
                                 ValuesIn(kSqlPolicyRulePreconditionExamples)));

TEST_P(DecodeSqlTagTransformOpRuleTest, DecodingTests) {
  // Setup
  const auto& [rule_name, result, precondition] = GetParam();
  ExpressionArena expr = GetExpressionArena(GetParam());
  IrProgramParserResult parse_result = GetParsedModule(GetParam());
  const ir::Module& module = *parse_result.module;
  const TagTransformOp* tag_transform_op =
      FindOperation<TagTransformOp>(module);

  // Check
  InferenceRules rules = DecodeSqlPolicyRules(expr, module, kTagNames);
  // Action name is rule_name appended with _0
  std::string action_name = absl::StrCat(rule_name, "_0");

  // Check the action rules are as expected.
  EXPECT_THAT(
      rules.action_rules(),
      UnorderedElementsAre(std::make_pair(
          action_name,
          // There is only an entry for output 0. The sequence of inference
          // rules consists of `CopyInput` followed by inference rule
          // corresponding to the test parameter.
          OutputRules(
              {{DecoderContext::kDefaultOutputIndex,
                InferenceRuleSequence({
                    {.conclusion = CopyInput{.input_index = 0}, .premises = {}},
                    {.conclusion = result.conclusion,
                     .premises = precondition.premises},
                })}}))));
  // Check that the only tag_transform_op should be marked as the action
  // corresponding to the rule.
  EXPECT_THAT(rules.actions(), UnorderedElementsAre(std::make_pair(
                                   tag_transform_op, action_name)));
}

TEST(DecodeSqlTagTransformOpRuleTest, DuplicateRuleNamesAreUniqued) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText(R"(sql_policy_rules {
      name: "Rule0"
      result {
        action: REMOVE_CONFIDENTIALITY
        tag: "A"
      }
  })");
  IrProgramParserResult parse_result = parser::ir::ParseProgram(R"(
    module m0 {
      block b0 {
        %0, %1, %2, %3, %4, %5 = core.multi_constant[] ()
        %6 = sql.tag_transform [rule_name: "Rule0"](%0, %1, %2)
        %7 = sql.tag_transform [rule_name: "Rule0"](%3, %4, %5)
      }
    }
  )");
  const ir::Module& module = *parse_result.module;
  std::vector<const TagTransformOp*> tag_transform_ops =
      FindAllOperations<TagTransformOp>(module);

  InferenceRules rules = DecodeSqlPolicyRules(expr, module);
  EXPECT_THAT(rules.action_rules(), SizeIs(2));
  EXPECT_THAT(utils::ranges::keys(rules.action_rules()),
              UnorderedElementsAre("Rule0_0", "Rule0_1"));
  EXPECT_THAT(utils::ranges::keys(rules.actions()),
              UnorderedElementsAreArray(tag_transform_ops));
  EXPECT_THAT(utils::ranges::values(rules.actions()),
              UnorderedElementsAre("Rule0_0", "Rule0_1"));
}

struct MergeOpInputIndicesExample {
  uint64_t control_input_start_index;
  std::vector<uint64_t> input_indices;
};

struct MergeOpConclusionExample {
  std::string rule_name;
  MergeIntegrityTags::Kind kind;
};

// Parameterized by the control_input_start_index and special rule name.
class DecodeSqlMergeOpRuleTest
    : public ::testing::TestWithParam<
          std::tuple<MergeOpInputIndicesExample, MergeOpConclusionExample>> {
 public:
  IrProgramParserResult GetParsedModule(uint64_t control_input_start_index,
                                        absl::string_view rule_name) {
    std::string module_text =
        absl::Substitute(R"(
    module m0 {
      block b0 {
        %0, %1, %2, %3, %4 = core.multi_constant[] ()
        %5 = sql.merge [control_input_start_index: $0] (%0, %1, %2, %3, %4)
        %6 = sql.tag_transform [rule_name: "$1"](%5)
      }
    }
  )",
                         control_input_start_index, rule_name);
    return parser::ir::ParseProgram(module_text);
  }
};

const MergeOpInputIndicesExample kMergeOpInputIndicesExamples[] = {
    {.control_input_start_index = 0, .input_indices = {}},
    {.control_input_start_index = 1, .input_indices = {0}},
    {.control_input_start_index = 2, .input_indices = {0, 1}},
    {.control_input_start_index = 3, .input_indices = {0, 1, 2}},
    {.control_input_start_index = 4, .input_indices = {0, 1, 2, 3}},
};

const MergeOpConclusionExample kMergeOpConclusionExamples[] = {
    {.rule_name = std::string(TagTransformOp::kIntersectItagRule),
     .kind = MergeIntegrityTags::Kind::kIntersect},
    {.rule_name = std::string(TagTransformOp::kUnionItagRule),
     .kind = MergeIntegrityTags::Kind::kUnion}};

INSTANTIATE_TEST_SUITE_P(DecodeSqlMergeOpRuleTests, DecodeSqlMergeOpRuleTest,
                         Combine(ValuesIn(kMergeOpInputIndicesExamples),
                                 ValuesIn(kMergeOpConclusionExamples)));

TEST_P(DecodeSqlMergeOpRuleTest, DecodingTests) {
  // Setup
  const auto& [input_index, rule] = GetParam();
  ExpressionArena expr = ParseSqlPolicyRulesProtoText("sql_policy_rules {}");
  IrProgramParserResult parse_result =
      GetParsedModule(input_index.control_input_start_index, rule.rule_name);
  const ir::Module& module = *parse_result.module;
  const MergeOp* merge_op = FindOperation<MergeOp>(module);

  InferenceRules rules = DecodeSqlPolicyRules(expr, module);

  // Action name is rule_name appended with _0
  std::string action_name = absl::StrCat(rule.rule_name, "_0");

  // Check the action rules are as expected.
  EXPECT_THAT(
      rules.action_rules(),
      UnorderedElementsAre(std::make_pair(
          action_name,
          // There is only an entry for output 0. The sequence of inference
          // rules consists of `CopyInput` followed by inference rule
          // corresponding to the test parameter.
          OutputRules(
              {{0, InferenceRuleSequence(
                       {{.conclusion =
                             MergeIntegrityTags{
                                 .kind = rule.kind,
                                 .input_indices = input_index.input_indices},
                         .premises = {}}})}}))));
  // Check that the only tag_transform_op should be marked as the action
  // corresponding to the rule.
  EXPECT_THAT(rules.actions(),
              UnorderedElementsAre(std::make_pair(merge_op, action_name)));
}
TEST(DecodeSqlPolicyRulesDeathTest, UnknownSqlPolicyRuleActionCausesFailure) {
  // Error: No action specified in result.
  ExpressionArena expr = ParseSqlPolicyRulesProtoText(R"(
  sql_policy_rules {
      name: "Rule0"
      result {
        tag: "A"
      }
  })");
  IrProgramParserResult parse_result = parser::ir::ParseProgram(R"(
    module m0 {
          block b0 {
            %0, %1, %2 = core.multi_constant[] ()
            %3 = sql.tag_transform [rule_name: "Rule0"](%0, %1, %2)
          }
        })");
  parser::ir::ParseProgram("module m0 { block b0 {} }");
  EXPECT_DEATH(DecodeSqlPolicyRules(expr, *parse_result.module),
               "Unknown category in SqlPolicyRule::Result::Action");
}

TEST(DecodeSqlPolicyRulesDeathTest, DuplicateRulesInProtoCausesFailure) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText(R"(
  sql_policy_rules {
      name: "Rule0"
      result {
        action: REMOVE_CONFIDENTIALITY
        tag: "A"
      }
  }
  sql_policy_rules {
      name: "Rule0"
      result {
        action: ADD_CONFIDENTIALITY
        tag: "A"
      }
  }
  )");

  IrProgramParserResult parse_result =
      parser::ir::ParseProgram("module m0 { block b0 {} }");
  EXPECT_DEATH(DecodeSqlPolicyRules(expr, *parse_result.module),
               "Dupliate entry for rule name 'Rule0'");
}

TEST(DecodeSqlPolicyRulesDeathTest, FailsIfInputIndexOfPreconditionIsAbsent) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText(R"(
  sql_policy_rules {
      name: "Rule0"
      result {
        action: REMOVE_CONFIDENTIALITY
        tag: "A"
      }
      preconditions {
        argument: "arg_b"
        required_integrity_tag: "B"
      }
  }
  )");
  IrProgramParserResult parse_result = parser::ir::ParseProgram(R"(
        module m0 {
          block b0 {
            %0, %1, %2 = core.multi_constant[] ()
            // Error: No entry for arg_b
            %3 = sql.tag_transform [rule_name: "Rule0", arg_a: 1](%0, %1, %2)
          }
        })");

  EXPECT_DEATH(DecodeSqlPolicyRules(expr, *parse_result.module),
               "Unable to find input index for 'arg_b'");
}

TEST(DecodeSqlPolicyRulesDeathTest,
     FailsIfSpecialTagTransformHasMoreThanOneInput) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText("sql_policy_rules {}");
  for (const auto& rule_name :
       {TagTransformOp::kUnionItagRule, TagTransformOp::kIntersectItagRule}) {
    IrProgramParserResult parse_result =
        parser::ir::ParseProgram(absl::Substitute(R"(
        module m0 {
          block b0 {
            %0, %1, %2 = core.multi_constant[] ()
            // Error: More than one argument.
            %3 = sql.tag_transform [rule_name: "$0"](%0, %1, %2)
          }
        })",
                                                  rule_name));
    EXPECT_DEATH(DecodeSqlPolicyRules(expr, *parse_result.module),
                 "Special tag transform op has more than one input");
  }
}

TEST(DecodeSqlPolicyRulesDeathTest,
     FailsIfSpecialTagTransformDoesNotHaveOperationResultAsArgument) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText("sql_policy_rules {}");
  for (const auto& rule_name :
       {TagTransformOp::kUnionItagRule, TagTransformOp::kIntersectItagRule}) {
    IrProgramParserResult parse_result =
        parser::ir::ParseProgram(absl::Substitute(R"(
        module m0 {
          block b0 {
            %0, %1, %2 = core.multi_constant[] ()
            // Error: tag_transform should point to an operation result.
            %3 = sql.tag_transform [rule_name: "$0"](<<ANY>>)
          }
        })",
                                                  rule_name));
    EXPECT_DEATH(
        DecodeSqlPolicyRules(expr, *parse_result.module),
        "Argument of a special tag_transform_op is not an operation result.");
  }
}

TEST(DecodeSqlPolicyRulesDeathTest,
     FailsIfSpecialTagTransformDoesNotReferToMergeOp) {
  ExpressionArena expr = ParseSqlPolicyRulesProtoText("sql_policy_rules {}");
  for (const auto& rule_name :
       {TagTransformOp::kUnionItagRule, TagTransformOp::kIntersectItagRule}) {
    IrProgramParserResult parse_result =
        parser::ir::ParseProgram(absl::Substitute(R"(
        module m0 {
          block b0 {
            %0, %1, %2 = core.multi_constant[] ()
            // Error: tag_transform should point to a merge op.
            %3 = sql.tag_transform [rule_name: "$0"](%0)
          }
        })",
                                                  rule_name));
    EXPECT_DEATH(DecodeSqlPolicyRules(expr, *parse_result.module),
                 "Special tag_transform_op is not referring to a MergeOp");
  }
}

}  // namespace
}  // namespace raksha::frontends::sql
