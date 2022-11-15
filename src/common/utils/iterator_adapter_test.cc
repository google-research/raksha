#include "src/common/utils/iterator_adapter.h"

#include <string>

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/iterator_range.h"

namespace raksha::utils {
namespace {

using ::testing::Combine;
using ::testing::ElementsAre;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

struct Stringifier {
  std::string operator()(int x, absl::string_view prefix) const {
    return std::string(prefix) + std::to_string(x);
  }
  std::string operator()(int x) const { return std::to_string(x); }
};

struct AddSomething {
  int operator()(int x, int value) const { return x + value; }
};

TEST(IteratorAdapterTest, MakeRangeWorksWithContainer) {
  std::vector<int> inputs = {1, 2, 3, 4, 5};
  EXPECT_THAT(make_adapted_range<Stringifier>(inputs),
              ElementsAre("1", "2", "3", "4", "5"));

  EXPECT_THAT(make_adapted_range<Stringifier>(inputs, "$"),
              ElementsAre("$1", "$2", "$3", "$4", "$5"));

  EXPECT_THAT(make_adapted_range<AddSomething>(inputs, 5),
              ElementsAre(6, 7, 8, 9, 10));

  EXPECT_THAT(make_adapted_range<AddSomething>(inputs, -1),
              ElementsAre(0, 1, 2, 3, 4));
}

TEST(IteratorAdapterTest, MakeRangeWorksWithIteratorRanges) {
  std::vector<int> inputs = {1, 2, 3, 4, 5};
  EXPECT_THAT(
      make_adapted_range<Stringifier>(inputs.begin(), inputs.begin() + 2),
      ElementsAre("1", "2"));
  EXPECT_THAT(make_adapted_range<Stringifier>(inputs.begin() + 1,
                                              inputs.begin() + 4, "-"),
              ElementsAre("-2", "-3", "-4"));
}

TEST(IteratorAdapterTest, MultipleArgumentMakeRangeWorks) {
  std::vector<int> inputs = {1, 2, 3, 4, 5};
  EXPECT_THAT(
      make_adapted_range<Stringifier>(inputs.begin(), inputs.begin() + 2),
      ElementsAre("1", "2"));
  EXPECT_THAT(
      make_adapted_range<Stringifier>(inputs.begin(), inputs.begin() + 4, "?"),
      ElementsAre("?1", "?2", "?3", "?4"));
}

// Set up some values for equality comparison.
namespace test_values {
std::vector<int> inputs = {1, 2, 3, 4, 5};
auto dollar_context1 = make_adapted_range<Stringifier>(inputs, "$");
auto dollar_context2 = make_adapted_range<Stringifier>(inputs, "$");
auto plus_context1 = make_adapted_range<Stringifier>(inputs, "+");
auto plus_context2 = make_adapted_range<Stringifier>(inputs, "+");
using AdaptedIterator = decltype(plus_context2.begin());
std::pair<AdaptedIterator, uint64_t> kSampleValues[] = {
    {plus_context1.begin(), 1},   {plus_context2.begin(), 1},
    {plus_context1.end(), 2},     {plus_context2.end(), 2},
    {dollar_context1.begin(), 3}, {dollar_context2.begin(), 3},
    {dollar_context1.end(), 4},   {dollar_context2.end(), 4},
};
}  // namespace test_values

class IteratorAdapterEqualityTest
    : public TestWithParam<
          std::tuple<std::pair<test_values::AdaptedIterator, uint64_t>,
                     std::pair<test_values::AdaptedIterator, uint64_t>>> {};

TEST_P(IteratorAdapterEqualityTest, EqualityAndInEqualityTests) {
  auto &[iterator_and_eq_class1, iterator_and_eq_class2] = GetParam();
  auto &[iterator1, eq_class1] = iterator_and_eq_class1;
  auto &[iterator2, eq_class2] = iterator_and_eq_class2;

  EXPECT_EQ(iterator1 == iterator2, eq_class1 == eq_class2);
}

INSTANTIATE_TEST_SUITE_P(IteratorAdapterEqualityTest,
                         IteratorAdapterEqualityTest,
                         Combine(ValuesIn(test_values::kSampleValues),
                                 ValuesIn(test_values::kSampleValues)));

}  // namespace
}  // namespace raksha::utils
