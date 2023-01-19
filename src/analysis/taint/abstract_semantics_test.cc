#include "src/analysis/taint/abstract_semantics.h"

#include <cstdint>
#include <vector>

#include "src/analysis/taint/abstract_ifc_tags.h"
#include "src/analysis/taint/inference_rule.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/analysis/taint/inference_rules_builder.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/module.h"
#include "src/ir/storage.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/value.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::analysis::taint {
namespace {

using ::testing::Combine;
using ::testing::Each;
using ::testing::Pointwise;
using ::testing::SizeIs;
using ::testing::Values;
using ::testing::ValuesIn;

using OutputRules = InferenceRules::OutputRules;
using InferenceRuleSequence = InferenceRules::InferenceRuleSequence;

// Update kTag... constants if you update kTagNames.
const std::vector<std::string> kTagNames = {"A", "B", "C", "D", "E"};
constexpr TagId kTagA = 0;
constexpr TagId kTagB = 1;
constexpr TagId kTagC = 2;
constexpr TagId kTagD = 3;
constexpr TagId kTagE = 4;

MATCHER_P(IsEquivalentTo, result, "") {
  if (arg.IsEquivalentTo(result)) return true;
  *result_listener << "Expected: " << result.ToString(kTagNames)
                   << " Actual: " << arg.ToString(kTagNames);
  return false;
}

MATCHER(ElementIsEquivalentTo, "") {
  const auto& [actual, expected] = arg;
  if (actual.IsEquivalentTo(expected)) return true;
  *result_listener << "Expected: " << expected.ToString(kTagNames)
                   << " Actual: " << actual.ToString(kTagNames);
  return false;
}

// Base class helper for all the abstract semantics tests.
class AbstractSemanticsTest : public ::testing::Test {
 public:
  static constexpr uint64_t kMaxOutputs = 5;
  AbstractSemanticsTest() {
    // Builds a few operators for use in tests.
    for (uint64_t n : {1, 2, 3, 4, 5}) {
      operators_.push_back(ir::Operator(absl::StrCat("Operator", n), n));
    }
  }

  // Builds an `AbstractSemantics` instance using the given `InferenceRules`,
  // invokes the operation transformer on the given operation with the given
  // inputs, and returns the results.
  std::vector<AbstractIfcTags> InvokeOperationTransformer(
      InferenceRules rules, const ir::Operation& operation,
      const std::vector<AbstractIfcTags>& inputs) {
    AbstractSemantics semantics(std::move(rules));
    return semantics.ApplyOperationTransformer(operation, inputs);
  }

 protected:
  // Creates an operation with given number of inputs and outputs.
  ir::Operation BuildOperation(uint64_t num_inputs, uint64_t num_outputs) {
    CHECK(num_outputs >= 1 && num_outputs < kMaxOutputs);
    std::vector any_inputs(num_inputs, ir::Value(ir::value::Any()));
    return ir::Operation(nullptr, operators_[num_outputs - 1], {}, any_inputs);
  }

  // Create some operators ordered by number of return values.
  std::vector<ir::Operator> operators_;
};

TEST(AbstractSemanticsTest, GetInitialStateReturnsEmptySets) {
  ir::types::TypeFactory type_factory;
  ir::Storage storage("some_storage", type_factory.MakePrimitiveType());
  ir::Operator plus_operator("core.plus");
  ir::Operation operation(nullptr, plus_operator, {}, {});
  ir::Block block;
  AbstractSemantics semantics(InferenceRules({}, {}, {}, {}));

  for (const ir::Value& value :
       {ir::Value(ir::value::Any()), ir::Value(ir::value::StoredValue(storage)),
        ir::Value(ir::value::OperationResult(operation, 0)),
        ir::Value(ir::value::OperationResult(operation, 5)),
        ir::Value(ir::value::BlockArgument(block, 0)),
        ir::Value(ir::value::BlockArgument(block, 2))}) {
    EXPECT_TRUE(semantics.GetInitialState(value).IsEquivalentTo(
        AbstractIfcTags({}, {})));
  }
}

// A structure to capture input state vector for testing operation transformers.
struct InputExample {
  // Inputs vector.
  std::vector<AbstractIfcTags> inputs;
  // Result of default behavior.
  AbstractIfcTags default_result;
  // Example premises. The first component of the pair indicates whether the
  // premises are satisfied by the input vector or not.
  std::vector<std::pair<bool, std::vector<InputHasIntegrity>>> premises;
};

// Return the input example with the given key in kInputExamples constant
// defined below.
const InputExample& GetInputExample(absl::string_view key);

// A structure to capture the result of applying conclusions to input examples.
struct AbstractSemanticsTestCase {
  // The name of the test.
  std::string test_name;
  // The key used to identify the input example.
  absl::string_view input_example_key;
  // The conclusions to test for.
  std::vector<Conclusion> conclusions;
  // The result of applying the conclusions to the given input example.
  AbstractIfcTags result;
};

class DefaultBehaviorTest
    : public AbstractSemanticsTest,
      public ::testing::WithParamInterface<std::tuple<
          /*num_outputs=*/uint64_t,
          /*examples=*/absl::flat_hash_map<absl::string_view,
                                           InputExample>::value_type>> {};

TEST_P(DefaultBehaviorTest, NoInferenceRuleResultsInDefaultBehavior) {
  const auto& [num_outputs, input_example_entry] = GetParam();
  const auto& [test_name, input_example] = input_example_entry;

  std::vector<AbstractIfcTags> inputs = input_example.inputs;
  ir::Operation op = BuildOperation(inputs.size(), num_outputs);
  std::vector<AbstractIfcTags> outputs =
      InvokeOperationTransformer(InferenceRules({}, {}, {}, {}), op, inputs);

  EXPECT_THAT(outputs, SizeIs(num_outputs));
  EXPECT_THAT(outputs, Each(IsEquivalentTo(input_example.default_result)));
}

class InferenceRuleInterpretationTest
    : public AbstractSemanticsTest,
      public ::testing::WithParamInterface<
          std::tuple<uint64_t, AbstractSemanticsTestCase>> {
 protected:
  // Builds the inference rules based on the test case, invokes the operation
  // transformer, and returns the results.
  std::vector<AbstractIfcTags> InvokeOperationTransformer(
      uint64_t num_outputs, uint64_t output_index,
      const AbstractSemanticsTestCase& test_case,
      const std::vector<InputHasIntegrity>& premises) {
    CHECK(output_index >= 0 && output_index < num_outputs);

    const std::vector<AbstractIfcTags>& inputs =
        GetInputExample(test_case.input_example_key).inputs;
    ir::Operation operation = BuildOperation(inputs.size(), num_outputs);

    // Build inference rules first.
    InferenceRulesBuilder builder(kTagNames);
    InferenceRuleSequence rules_sequence = utils::MapIter<InferenceRule>(
        test_case.conclusions, [&premises](const Conclusion& conclusion) {
          return InferenceRule{.conclusion = conclusion, .premises = premises};
        });
    builder.MarkOperationAsAction(operation, "SomeAction");
    builder.AddOutputRulesForAction("SomeAction",
                                    {{output_index, rules_sequence}});

    // Apply the operation transformer.
    return AbstractSemanticsTest::InvokeOperationTransformer(builder.Build(),
                                                             operation, inputs);
  }
};

TEST_P(InferenceRuleInterpretationTest, OutputRulesAreApplied) {
  const auto& [output_index, test_case] = GetParam();
  const InputExample& input_example =
      GetInputExample(test_case.input_example_key);
  uint64_t num_outputs = 4;

  for (const auto& [satisfied, premises] : input_example.premises) {
    // make as many copies of default result as there are outputs.
    std::vector<AbstractIfcTags> expected_outputs(num_outputs,
                                                  input_example.default_result);
    if (satisfied) {
      // Replace the output of inference rule with the result of the test case.
      expected_outputs[output_index] = test_case.result;
    }

    std::vector<AbstractIfcTags> outputs = InvokeOperationTransformer(
        num_outputs, output_index, test_case, premises);

    EXPECT_THAT(outputs, SizeIs(num_outputs));
    EXPECT_THAT(outputs, Pointwise(ElementIsEquivalentTo(), expected_outputs));
  }
}

constexpr absl::string_view kNoInputs = "NoInputs";
constexpr absl::string_view kAllBottomReturnsBottom = "AllBottomReturnsBottom";
constexpr absl::string_view kBottom01 = "Bottom01";
constexpr absl::string_view kBottom02 = "Bottom02";
constexpr absl::string_view kIntegrityIsClearedEvenWithBottom =
    "IntegrityIsClearedEvenWithBottom";
constexpr absl::string_view kSecrecyIsUnioned01 = "SecrecyIsUnioned01";
constexpr absl::string_view kSecrecyIsUnioned02 = "SecrecyIsUnioned02";
constexpr absl::string_view kIntegrityIsCleared01 = "IntegrityIsCleared01";
constexpr absl::string_view kIntegrityIsCleared02 = "IntegrityIsCleared02";
constexpr absl::string_view kSecrecyIsUnionedIntegrityIsCleared =
    "SecrecyIsUnionedIntegrityIsCleared";

// Some example inputs.
const absl::flat_hash_map<absl::string_view, InputExample> kInputExamples = {
    {kNoInputs,
     {.inputs = {},
      // If there are no inputs, we treat it as defining the initial value.
      .default_result = AbstractIfcTags({}, {}),
      .premises = {{false, {}}}}},
    {kAllBottomReturnsBottom,
     {.inputs = {AbstractIfcTags::Bottom(), AbstractIfcTags::Bottom()},
      .default_result = {AbstractIfcTags::Bottom()},
      // False is not possible.
      .premises = {{true, {InputHasIntegrity{.input_index = 0, .tag = kTagA}}},
                   {true, {}}}}},
    {kBottom01,
     {.inputs = {AbstractIfcTags::Bottom(), AbstractIfcTags({}, {})},
      .default_result = {AbstractIfcTags({}, {})},
      .premises = {{true, {InputHasIntegrity{.input_index = 0, .tag = kTagA}}},
                   {false,
                    {InputHasIntegrity{.input_index = 1, .tag = kTagB}}}}}},
    {kBottom02,
     {.inputs = {AbstractIfcTags({kTagA}, {kTagC}), AbstractIfcTags::Bottom()},
      .default_result = {AbstractIfcTags({kTagA}, {})},
      .premises = {{true, {InputHasIntegrity{.input_index = 0, .tag = kTagC}}},
                   {false,
                    {InputHasIntegrity{.input_index = 0, .tag = kTagA}}}}}},
    {kIntegrityIsClearedEvenWithBottom,
     {.inputs = {AbstractIfcTags({kTagA}, {kTagA}), AbstractIfcTags::Bottom()},
      .default_result = {AbstractIfcTags({kTagA}, {})},
      .premises = {{true, {InputHasIntegrity{.input_index = 0, .tag = kTagA}}},
                   {false,
                    {InputHasIntegrity{.input_index = 0, .tag = kTagA},
                     InputHasIntegrity{.input_index = 1, .tag = kTagB}}}}}},
    {kSecrecyIsUnioned01,
     {.inputs = {AbstractIfcTags::Bottom(), AbstractIfcTags({}, {}),
                 AbstractIfcTags({kTagA}, {})},
      .default_result = {AbstractIfcTags({kTagA}, {})},
      .premises = {{true, {InputHasIntegrity{.input_index = 2, .tag = kTagA}}},
                   {false,
                    {InputHasIntegrity{.input_index = 1, .tag = kTagB}}}}}},
    {kSecrecyIsUnioned02,
     {.inputs = {AbstractIfcTags({kTagA}, {}),
                 AbstractIfcTags({kTagA, kTagB}, {}),
                 AbstractIfcTags({kTagC, kTagD}, {}),
                 AbstractIfcTags({kTagA}, {})},
      .default_result = {AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {})},
      .premises = {{true,
                    {InputHasIntegrity{.input_index = 1, .tag = kTagA},
                     InputHasIntegrity{.input_index = 1, .tag = kTagB}}},
                   {false,
                    {InputHasIntegrity{.input_index = 2, .tag = kTagD},
                     InputHasIntegrity{.input_index = 2, .tag = kTagB}}}}}},
    {kIntegrityIsCleared01,
     {.inputs = {AbstractIfcTags({}, {kTagB}), AbstractIfcTags({}, {kTagC}),
                 AbstractIfcTags({}, {kTagB}), AbstractIfcTags({}, {kTagB})},
      .default_result = {AbstractIfcTags({}, {})},
      .premises =
          {
              {true,
               {InputHasIntegrity{.input_index = 0, .tag = kTagB},
                InputHasIntegrity{.input_index = 1, .tag = kTagC}}},
              {false,
               {InputHasIntegrity{.input_index = 2, .tag = kTagB},
                InputHasIntegrity{.input_index = 3, .tag = kTagC}}},
          }}},
    {kIntegrityIsCleared02,
     {.inputs = {AbstractIfcTags({kTagA}, {kTagB}),
                 AbstractIfcTags({}, {kTagB}),
                 AbstractIfcTags({}, {kTagB, kTagC}),
                 AbstractIfcTags({}, {kTagB, kTagD})},
      .default_result = {AbstractIfcTags({kTagA}, {})},
      .premises = {{true, {InputHasIntegrity{.input_index = 0, .tag = kTagB}}},
                   {false,
                    {InputHasIntegrity{.input_index = 0, .tag = kTagB},
                     InputHasIntegrity{.input_index = 1, .tag = kTagB},
                     InputHasIntegrity{.input_index = 2, .tag = kTagD}}}}}},
    {kSecrecyIsUnionedIntegrityIsCleared,
     {.inputs = {AbstractIfcTags({kTagA}, {kTagB}),
                 AbstractIfcTags({kTagA, kTagB}, {kTagA, kTagB, kTagC}),
                 AbstractIfcTags({kTagC, kTagD}, {kTagC}),
                 AbstractIfcTags({kTagA}, {kTagC, kTagD})},
      .default_result = {AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {})},
      .premises = {{true,
                    {InputHasIntegrity{.input_index = 0, .tag = kTagB},
                     InputHasIntegrity{.input_index = 1, .tag = kTagB},
                     InputHasIntegrity{.input_index = 2, .tag = kTagC},
                     InputHasIntegrity{.input_index = 3, .tag = kTagD}}},
                   {false,
                    {InputHasIntegrity{.input_index = 0, .tag = kTagA},
                     InputHasIntegrity{.input_index = 1, .tag = kTagB},
                     InputHasIntegrity{.input_index = 2, .tag = kTagC},
                     InputHasIntegrity{.input_index = 3, .tag = kTagD}}}}}}};

const InputExample& GetInputExample(absl::string_view key) {
  CHECK(kInputExamples.contains(key))
      << "Key '" << key << "' not found in input examples.";
  return kInputExamples.at(key);
}

const AbstractSemanticsTestCase kAbstractSemanticsTestExamples[] = {
    // CopyInputExamples
    {.test_name = "CopyInput01",
     .input_example_key = "AllBottomReturnsBottom",
     .conclusions = {},
     .result = {AbstractIfcTags::Bottom()}},
    {.test_name = "CopyInput02",
     .input_example_key = "AllBottomReturnsBottom",
     .conclusions = {CopyInput{.input_index = 0}},
     .result = {AbstractIfcTags::Bottom()}},
    {.test_name = "CopyInput03",
     .input_example_key = kBottom01,
     .conclusions = {CopyInput{.input_index = 0}},
     .result = GetInputExample(kBottom01).inputs[0]},
    {.test_name = "CopyInput04",
     .input_example_key = kBottom01,
     .conclusions = {CopyInput{.input_index = 1}},
     .result = GetInputExample(kBottom01).inputs[1]},
    {.test_name = "CopyInput05",
     .input_example_key = kBottom01,
     .conclusions = {CopyInput{.input_index = 1}, CopyInput{.input_index = 0}},
     .result = GetInputExample(kBottom01).inputs[0]},
    {.test_name = "CopyInput06",
     .input_example_key = kBottom02,
     .conclusions = {CopyInput{.input_index = 1}, CopyInput{.input_index = 0}},
     .result = GetInputExample(kBottom02).inputs[0]},
    {.test_name = "CopyInput07",
     .input_example_key = "SecrecyIsUnionedIntegrityIsCleared",
     .conclusions{CopyInput{.input_index = 0}},
     .result = GetInputExample("SecrecyIsUnionedIntegrityIsCleared").inputs[0]},
    {.test_name = "CopyInput08",
     .input_example_key = "SecrecyIsUnionedIntegrityIsCleared",
     .conclusions{CopyInput{.input_index = 2}},
     .result = GetInputExample("SecrecyIsUnionedIntegrityIsCleared").inputs[2]},
    {.test_name = "CopyInput09",
     .input_example_key = "SecrecyIsUnionedIntegrityIsCleared",
     .conclusions{CopyInput{.input_index = 2}, CopyInput{.input_index = 1}},
     .result = GetInputExample("SecrecyIsUnionedIntegrityIsCleared").inputs[1]},
    //  ModifyTag
    {.test_name = "ModifyTag01",
     .input_example_key = kAllBottomReturnsBottom,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagA}},
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "ModifyTag02",
     .input_example_key = kAllBottomReturnsBottom,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy,
                               .tag = kTagA}},
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "ModifyTag03",
     .input_example_key = kBottom01,
     .conclusions = {{ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                                .tag = kTagA}}},
     .result = AbstractIfcTags({}, {kTagA})},
    {.test_name = "ModifyTag04",
     .input_example_key = kBottom02,
     .conclusions = {{ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy,
                                .tag = kTagA}}},
     .result = AbstractIfcTags({kTagA}, {})},
    {.test_name = "ModifyTag05",
     .input_example_key = kBottom02,
     .conclusions = {{ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                                .tag = kTagA}}},
     .result = AbstractIfcTags({kTagA}, {kTagA})},
    {.test_name = "ModifyTag06",
     .input_example_key = kBottom02,
     .conclusions =
         {{ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity, .tag = kTagA},
           ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy, .tag = kTagA}}},
     .result = AbstractIfcTags({}, {kTagA})},
    {.test_name = "ModifyTag07",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagA}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagA})},
    {.test_name = "ModifyTag08",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagA}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagA})},
    {.test_name = "ModifyTag09",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagA},
                     ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagE}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagA, kTagE})},
    {.test_name = "ModifyTag10",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy,
                               .tag = kTagD}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC}, {})},
    {.test_name = "ModifyTag11",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy,
                               .tag = kTagE}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD, kTagE}, {})},
    {.test_name = "ModifyTag12",
     .input_example_key = kIntegrityIsCleared02,
     .conclusions = {CopyInput{.input_index = 0},
                     ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy,
                               .tag = kTagE}},
     .result = AbstractIfcTags({kTagA, kTagE}, {kTagB})},
    {.test_name = "ModifyTag13",
     .input_example_key = kIntegrityIsCleared02,
     .conclusions = {CopyInput{.input_index = 0},
                     ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy,
                               .tag = kTagA}},
     .result = AbstractIfcTags({}, {kTagB})},
    {.test_name = "ModifyTag14",
     .input_example_key = kIntegrityIsCleared02,
     .conclusions = {CopyInput{.input_index = 0},
                     ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity,
                               .tag = kTagE}},
     .result = AbstractIfcTags({kTagA}, {kTagB, kTagE})},
    // MergeIntegrityTags
    {.test_name = "MergeIntegrityTags01",
     .input_example_key = kAllBottomReturnsBottom,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {0, 1}}},
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "MergeIntegrityTags02",
     .input_example_key = kAllBottomReturnsBottom,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect,
         .input_indices = {0, 1}}},
     // Bottom if any input is bottom
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "MergeIntegrityTags03",
     .input_example_key = kBottom01,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect,
         .input_indices = {0, 1}}},
     // Bottom if any input is bottom
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "MergeIntegrityTags04",
     .input_example_key = kBottom02,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {0, 1}}},
     // Bottom if any input is bottom
     .result = AbstractIfcTags::Bottom()},
    {.test_name = "MergeIntegrityTags05",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {1}}},
     .result =
         AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagA, kTagB, kTagC})},
    {.test_name = "MergeIntegrityTags06",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {0, 1}}},
     .result =
         AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagA, kTagB, kTagC})},
    {.test_name = "MergeIntegrityTags07",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {0, 3}}},
     .result =
         AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagB, kTagC, kTagD})},
    {.test_name = "MergeIntegrityTags08",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kUnion, .input_indices = {0, 2, 3}}},
     .result =
         AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagB, kTagC, kTagD})},
    {.test_name = "MergeIntegrityTags09",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect, .input_indices = {0}}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagB})},
    {.test_name = "MergeIntegrityTags10",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect,
         .input_indices = {0, 3}}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {})},
    {.test_name = "MergeIntegrityTags11",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect,
         .input_indices = {2, 3}}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagC})},
    {.test_name = "MergeIntegrityTags12",
     .input_example_key = kSecrecyIsUnionedIntegrityIsCleared,
     .conclusions = {MergeIntegrityTags{
         .kind = MergeIntegrityTags::Kind::kIntersect,
         .input_indices = {1, 2, 3}}},
     .result = AbstractIfcTags({kTagA, kTagB, kTagC, kTagD}, {kTagC})}};

INSTANTIATE_TEST_SUITE_P(
    AbstractSemanticsTests, DefaultBehaviorTest,
    Combine(Values(1, 2, 4), ValuesIn(kInputExamples)),
    [](const testing::TestParamInfo<DefaultBehaviorTest::ParamType>& info) {
      // const auto& [num_outputs, input_example_entry] = info.param;
      return absl::StrCat(std::string(std::get<1>(info.param).first), "_",
                          std::get<0>(info.param));
    });

INSTANTIATE_TEST_SUITE_P(
    AbstractSemanticsTests, InferenceRuleInterpretationTest,
    Combine(Values(0, 1, 3), ValuesIn(kAbstractSemanticsTestExamples)),
    [](const testing::TestParamInfo<InferenceRuleInterpretationTest::ParamType>&
           info) {
      return absl::StrCat(std::get<1>(info.param).test_name, "_",
                          std::get<1>(info.param).input_example_key, "_",
                          std::get<0>(info.param));
    });

}  // namespace
}  // namespace raksha::analysis::taint
