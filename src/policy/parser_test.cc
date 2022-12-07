#include "src/policy/parser.h"

#include "src/common/testing/gtest.h"

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
    {R"(allow action "private" on "UserData")", true},
    {R"(allow action "private" if )", false},
    {R"(allow action "egress" on "UserData" by "Brella" if { data:dp_epsilon <
    ln(8) })",
     false},
    {R"(allow action "egress" on "UserData" by "Brella" if { data:dp_epsilon <
    "ln(8)\" })",
     true},
    {R"(allow action "egress" on "UserData" by actor if { startsWith(actor:name,
    "Trusted"), data:dp_epsilon < 3})",
     false},
    {R"(allow action "egress" on "UserData" by "Brella" if {
    isEgressableCategory("UserData", TRUE), data:dp_epsilon < 3 })",
     true},
    {R"(add "Private" to "UserData.name")", true},
    {R"(add "TimestampMillis" after "ReadTimeStampMillis")", false},
    {R"(after action "TimestampToDaysRedactor" {
                                  remove "TimestampMillis" from action.result[0]
    })",
     true},
    {R"(decl principal "Brella")", true},
    {R"(decl category "UserData")", true},
    {R"(decl action "egress")", false},
    {R"(decl action "egress" {})", true},
    {R"(decl action "egress" { Int roundSize, })", true},
    {R"(op is action "read"  with { args[0] := op.args[0] })", true},
    {R"(op is action "read" if {
        op.operator:name = "send_over_network",
        op.args[0]:constant = "some.untrusted.site"
    } with {
        args[0] := op.args[0] })",
     true},
    {R"(after action "Compare" {
  allow action "egress" on "TimestampMillis" by "Brella" if {
    action.data[0]:integrity = "TimestampMillis",
    action.data[1]:integrity = "TimestampMillis"
  }
})",
     true},
    {R"(decl action "read" {}
op is action "read" if {
  op.operator:name = "chronicle.read_connection"
} with {
  args[0] := op.args[0]
}
allow action "read" on  "PecanConversation" by "LiveTranslate" if {
  data:ttlInMins < 30
}
)",
     true},
    {R"(decl action "federated_analytics" {
    Int round_size,
}

op is action "federated_analytics" if {
  op.operator:name = "federated_analytics"
} with {
  args[0] := op.args[0],
  round_size := op.attributes["round_size"]
}

allow action "federated_analytics" on "SmartSelectStats" by "Brella" if {
 data:round_size >= 1000
}
)",
     true},
};

INSTANTIATE_TEST_SUITE_P(PolicyParserTest, PolicyParserTest,
                         ValuesIn(kTestStatements));
}  // namespace raksha::policy
