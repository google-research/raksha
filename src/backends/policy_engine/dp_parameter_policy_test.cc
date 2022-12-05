#include "src/backends/policy_engine/dp_parameter_policy.h"

#include <optional>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "src/common/testing/gtest.h"

namespace raksha::backends::policy_engine {
namespace {

using DpParameter = DpParameterPolicy::DpParameterNumericType;
using testing::Combine;
using testing::ValuesIn;

class DpParamaterPolicyRulePolicyTest
    : public testing::TestWithParam<std::tuple<DpParameter, DpParameter>> {
 protected:
  explicit DpParamaterPolicyRulePolicyTest()
      : epsilon_(std::get<0>(GetParam())),
        delta_(std::get<1>(GetParam())),
        policy_(epsilon_, delta_) {}

  DpParameter epsilon_;
  DpParameter delta_;
  DpParameterPolicy policy_;
};

TEST_P(DpParamaterPolicyRulePolicyTest, TestPolicyFactName) {
  std::optional<std::string> policy_fact_name = policy_.GetPolicyFactName();
  ASSERT_TRUE(policy_fact_name.has_value());
  EXPECT_EQ(*policy_fact_name, "isDPParameter");
}

// Convert a DpParameter to a string, checking our transformation to ensure that
// that string can be perfectly converted back to the input parameter. This may
// seems silly so long as `DpParameter`s are integers, but this will make more
// sense when we switch to floats, as %f and %g will print a fixed-precision
// (and thus possibly lossy) representation (unlike %a).
std::string ConvertParamToStringExpectLossless(DpParameter parameter) {
  std::string result = absl::StrFormat("%lu", parameter);
  DpParameter parsed_param = 0;
  EXPECT_TRUE(absl::SimpleAtoi(result, &parsed_param));
  EXPECT_EQ(parsed_param, parameter);
  return result;
}

// GetPolicyString constructs a Datalog string from the policy given the
// parameters. Here, we deconstruct that same policy to extract those parameters
// and ensure that they are what we expect.
TEST_P(DpParamaterPolicyRulePolicyTest, TestPolicyString) {
  std::optional<std::string> optional_policy_string = policy_.GetPolicyString();
  ASSERT_TRUE(optional_policy_string.has_value());
  std::string policy_string = *optional_policy_string;
  std::vector<std::string> lines =
      absl::StrSplit(policy_string, '\n', absl::SkipEmpty());
  EXPECT_EQ(lines.size(), 2);

  std::string epsilon_value_string =
      ConvertParamToStringExpectLossless(epsilon_);
  std::string delta_value_string = ConvertParamToStringExpectLossless(delta_);

  std::vector<std::string> expected_lines = {
      absl::StrFormat("$EpsilonValue(%s)", epsilon_value_string),
      absl::StrFormat("$DeltaValue(%s)", delta_value_string)};

  EXPECT_THAT(lines, testing::UnorderedElementsAreArray(expected_lines));
}

// Note: some of the parameter values will be duplicated until the DpParameter
// type switches to floating point numbers.
constexpr DpParameter kSampleParameters[] = {
    0,
    1,
    std::numeric_limits<DpParameter>::min(),
    std::numeric_limits<DpParameter>::max(),
    std::numeric_limits<DpParameter>::lowest(),
    std::numeric_limits<DpParameter>::epsilon()};

INSTANTIATE_TEST_SUITE_P(DpParameterPolicyRuleTest,
                         DpParamaterPolicyRulePolicyTest,
                         Combine(ValuesIn(kSampleParameters),
                                 ValuesIn(kSampleParameters)));

}  // namespace
}  // namespace raksha::backends::policy_engine
