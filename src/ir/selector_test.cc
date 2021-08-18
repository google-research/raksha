#include "src/ir/selector.h"

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir {

enum SelectorKind {
  kFieldSelector,
};

struct ExampleSelectorAndInfo {
  Selector selector;
  SelectorKind selector_kind;
  std::string original_str;
  std::string expected_to_string;
};

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

TEST_P(SelectorTest, ToStringAsExpected) {
  const ExampleSelectorAndInfo info = GetParam();

  ASSERT_EQ(info.selector.ToString(), info.expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(
    SelectorTestSuite,
    SelectorTest,
    testing::ValuesIn(example_selectors));

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

}  // namespace raksha::ir
