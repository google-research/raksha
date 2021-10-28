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
load("//build_defs:raksha.bzl", "policy_check")

licenses(["notice"])

policy_check(
    name = "delegation_granted",
    auth_logic = "ok_claim_propagates_downgrade_granted.authlogic",
    dataflow_graph = "ok_claim_propagates.arcs",
)

policy_check(
    name = "delegation_not_granted",
    auth_logic = "ok_claim_propagates_downgrade_not_granted.authlogic",
    dataflow_graph = "ok_claim_propagates.arcs",
    expect_failure = True,
)

policy_check(
    name = "delegation_granted_by_non_owner",
    auth_logic = "ok_claim_propagates_downgrade_granted_by_non_owner.authlogic",
    dataflow_graph = "ok_claim_propagates.arcs",
    expect_failure = True,
)
