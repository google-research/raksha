# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------------
load("//build_defs:raksha_policy_verifier.bzl", "raksha_policy_verifier_library")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

raksha_policy_verifier_library(
    name = "may_will_pass",
    policies = [
        "simple_may_will_pass.auth",
    ],
    policy_verifier_interfaces = [":simple_may_will.dl"],
)

raksha_policy_verifier_library(
    name = "may_will_fail",
    policies = [
        "simple_may_will_fail.auth",
    ],
    policy_verifier_interfaces = [":simple_may_will.dl"],
)

[sh_test(
    name = "check_policy_compliance_%s_test" % verifier_name,
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "%s" % test_result,
        "sql_policy_analysis",
        "ir",
        "$(location //src/analysis/souffle/examples:empty.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "%s" % verifier_name,
    ],
    data = [
        "//src/analysis/souffle/examples:empty.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
) for verifier_name, test_result in [
    ("may_will_pass_cxx", "pass"),
    ("may_will_fail_cxx", "fail"),
]]
