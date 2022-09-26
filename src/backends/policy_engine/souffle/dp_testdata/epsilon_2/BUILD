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
    "1q_nopm_failing.ir",
    "1q_join_failing.ir",
    "1q_join_passing_gb.ir",
    "1q_join_passing_kappa.ir",
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

[sh_test(
    name = "check_dp_policy_compliance_epsilon_%s_test" % ir_name,
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        test_result,
        "sql_policy_analysis",
        "ir",
        "$(location :%s.ir)" % ir_name,
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "dp_policy_verifier_epsilon_2_with_auth_logic_lib_cxx",
    ],
    data = [
        ":%s.ir" % ir_name,
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
) for ir_name, test_result in [
    ("1q_failing", "fail"),
    ("1q_passing", "pass"),
    ("2q_failing", "fail"),
    ("2q_passing", "pass"),
]]
