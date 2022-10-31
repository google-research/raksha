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

# policy_check(
#     name = "delegation_granted",
#     auth_logic = "ok_claim_propagates_downgrade_granted.auth",
#     dataflow_graph = "ok_claim_propagates.arcs",
# )

# policy_check(
#     name = "delegation_not_granted",
#     auth_logic = "ok_claim_propagates_downgrade_not_granted.auth",
#     dataflow_graph = "ok_claim_propagates.arcs",
#     expect_failure = True,
# )

# policy_check(
#     name = "delegation_granted_by_non_owner",
#     auth_logic = "ok_claim_propagates_downgrade_granted_by_non_owner.auth",
#     dataflow_graph = "ok_claim_propagates.arcs",
#     expect_failure = True,
# )

raksha_policy_verifier_library(
    name = "delegation_granted_downgrades",
    policies = [
        "ok_claim_propagates_downgrade_granted.auth",
    ],
    policy_verifier_interfaces = [":delegation_granted.dl"],
)

raksha_policy_verifier_library(
    name = "delegation_not_granted_downgrades",
    policies = [
        "ok_claim_propagates_downgrade_not_granted.auth",
    ],
    policy_verifier_interfaces = [":delegation_not_granted.dl"],
)

raksha_policy_verifier_library(
    name = "delegation_not_granted_by_non_owner",
    policies = [
        "ok_claim_propagates_downgrade_granted_by_non_owner.auth",
    ],
    policy_verifier_interfaces = [":delegation_granted_by_non_owner.dl"],
)

sh_test(
    name = "check_policy_compliance_delegation_granted_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location //src/analysis/souffle/examples:empty.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "delegation_granted_downgrades_cxx",
    ],
    data = [
        "//src/analysis/souffle/examples:empty.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_delegation_not_granted_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location //src/analysis/souffle/examples:empty.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "delegation_not_granted_downgrades_cxx",
    ],
    data = [
        "//src/analysis/souffle/examples:empty.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_delegation_granted_by_non_owner_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location //src/analysis/souffle/examples:empty.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "delegation_not_granted_by_non_owner_cxx",
    ],
    data = [
        "//src/analysis/souffle/examples:empty.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)
