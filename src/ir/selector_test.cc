#include "src/ir/selector.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir {

// An enumeration of the different specific types of selectors that may
// appear in a Selector.
enum SelectorKind {
  kFieldSelector,
};

// A struct consisting of an example Selector to be used as a test input and
// assorted information about that Selector, used for deriving our
// expectation of the test.
struct ExampleSelectorAndInfo {
  // The Selector to be used as test input.
  Selector selector;
  // The specific type of selector we expect to be in selector.
  SelectorKind selector_kind;
  // The original string from which the selector was constructed.
  std::string original_str;
  // The string we expect to be printed when ToString is called on selector.
  std::string expected_to_string;
};

// A group of example selectors which can be used as test inputs.
static const ExampleSelectorAndInfo example_selectors[] = {
    {
      .selector = Selector(FieldSelector("field1")),
      .selector_kind = kFieldSelector,
      .original_str = "field1",
      .expected_to_string = ".field1" },
    {
      .selector = Selector(FieldSelector("another_field")),
      .selector_kind = kFieldSelector,
      .original_str = "another_field",
      .expected_to_string = ".another_field" },
};

class SelectorTest : public ::testing::TestWithParam<ExampleSelectorAndInfo> {};

// Check that the ToString result is as we expect.
TEST_P(SelectorTest, ToStringAsExpected) {
  const ExampleSelectorAndInfo info = GetParam();
  ASSERT_EQ(info.selector.ToString(), info.expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(
    SelectorTestSuite,
    SelectorTest,
    testing::ValuesIn(example_selectors));

// Check that absl's hashing works correctly for the selectors in
// example_selectors.
TEST(SelectorHashTest, SelectorHashTest) {
  std::vector<Selector> selectors_to_check;
  for (ExampleSelectorAndInfo info : example_selectors) {
    selectors_to_check.push_back(info.selector);
  }
  ASSERT_TRUE(
      absl::VerifyTypeImplementsAbslHashCorrectly(selectors_to_check));
}

class SelectorPairTest : public
    ::testing::TestWithParam<
      std::tuple<ExampleSelectorAndInfo, ExampleSelectorAndInfo>> {};

// For each possible pair of selectors drawn from the selectors in
// example_selectors, ensure that the selectors compare equal only when they
// are the same kind of selector and are derived from equal input strings.
TEST_P(SelectorPairTest, SelectorEqTest) {
  const std::tuple<ExampleSelectorAndInfo, ExampleSelectorAndInfo> param =
      GetParam();
  const ExampleSelectorAndInfo info1 = std::get<0>(param);
  const ExampleSelectorAndInfo info2 = std::get<1>(param);

  bool const expect_equal =
      (info1.selector_kind == info2.selector_kind) &&
      (info1.original_str == info2.original_str);

  bool const actually_equal = info1.selector == info2.selector;
  ASSERT_EQ(actually_equal, expect_equal);
}

INSTANTIATE_TEST_SUITE_P(
    SelectorPariTestSuite,
    SelectorPairTest,
    testing::Combine(
        testing::ValuesIn(example_selectors),
        testing::ValuesIn(example_selectors)));

class RoundTripSelectorProtoTest :
 public testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(RoundTripSelectorProtoTest, RoundTripSelectorProtoTest) {
  arcs::AccessPathProto_Selector orig_selector_proto;
  const std::string &selector_as_textproto = std::get<0>(GetParam());
  const std::string &expected_selector_to_string = std::get<1>(GetParam());
  google::protobuf::TextFormat::ParseFromString(
      selector_as_textproto, &orig_selector_proto);
  Selector selector = Selector::CreateFromProto(orig_selector_proto);
  EXPECT_EQ(selector.ToString(), expected_selector_to_string);
  arcs::AccessPathProto_Selector result_selector_proto = selector.MakeProto();
  ASSERT_TRUE(
      google::protobuf::util::MessageDifferencer::Equals(
          orig_selector_proto, result_selector_proto));
}

static const std::tuple<std::string, std::string>
  selector_proto_and_tostring_pairs[] = {
    { "field: \"foo\"", ".foo" },
    { "field: \"x\"", ".x" },
    { "field: \"bar\"", ".bar"}
};

INSTANTIATE_TEST_SUITE_P(
    RoundTripSelectorProtoTest, RoundTripSelectorProtoTest,
    testing::ValuesIn(selector_proto_and_tostring_pairs));

}  // namespace raksha::ir
