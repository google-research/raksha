#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#----------------------------------------------------------------------------
load(
    "//build_defs:raksha_policy_verifier.bzl",
    "raksha_policy_verifier_library",
)

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

exports_files([
    "policy.facts",
    "1q_passing.ir",
    "1q_failing.ir",
    "1q_no_aggregation_failing.ir",
    "1q_no_aggregation_passing.ir",
    "2q_passing.ir",
    "2q_failing.ir",
])

raksha_policy_verifier_library(
    name = "dp_policy_verifier_epsilon_2_with_auth_logic_lib",
    additional_dl_files = [
        "//src/analysis/souffle:sql_output.dl",
        "//src/analysis/souffle:epsilon_analysis.dl",
        "//src/analysis/souffle:sensitivity_analysis.dl",
    ],
    policies = [":policy.auth"],
    policy_verifier_interfaces = ["//src/analysis/souffle:dp_policy_verifier_interface.dl"],
)

sh_test(
    name = "check_dp_policy_compliance_epsilon_1q_fail_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location :1q_failing.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "dp_policy_verifier_epsilon_2_with_auth_logic_lib_cxx",
    ],
    data = [
        ":1q_failing.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_dp_policy_compliance_epsilon_1q_pass_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "pass",
        "sql_policy_analysis",
        "ir",
        "$(location :1q_passing.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "dp_policy_verifier_epsilon_2_with_auth_logic_lib_cxx",
    ],
    data = [
        ":1q_passing.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_dp_policy_compliance_epsilon_2q_fail_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location :2q_failing.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "dp_policy_verifier_epsilon_2_with_auth_logic_lib_cxx",
    ],
    data = [
        ":2q_failing.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_dp_policy_compliance_epsilon_2q_pass_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "pass",
        "sql_policy_analysis",
        "ir",
        "$(location :2q_passing.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "dp_policy_verifier_epsilon_2_with_auth_logic_lib_cxx",
    ],
    data = [
        ":2q_passing.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)
