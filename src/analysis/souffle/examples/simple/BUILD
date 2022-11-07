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

<<<<<<< HEAD
=======
# To invoke policy engine, `bazel test :ok_claim_propagates_check`.
# raksha_policy_check(
#     name = "ok_claim_propagates_check",
#     src = "ok_claim_propagates.raksha",
# )

>>>>>>> 64068c8 (remove rust dependency for auth_logic in src/analysis/souffle/example/)
raksha_policy_verifier_library(
    name = "ok_claim_propagates_check",
    policies = [
        "ok_claim_propagates.auth",
    ],
    policy_verifier_interfaces = [":ok_claim_propagates_check.dl"],
)

sh_test(
    name = "check_policy_compliance_ok_claim_propagates_test",
    srcs = ["//src/backends/policy_engine/souffle:check_policy_compliance_test.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:check_policy_compliance)",
        "fail",
        "sql_policy_analysis",
        "ir",
        "$(location //src/analysis/souffle/examples:empty.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "ok_claim_propagates_check_cxx",
    ],
    data = [
        "//src/analysis/souffle/examples:empty.ir",
        "//src/backends/policy_engine/souffle:check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)
