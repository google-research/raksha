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
    name = "may_will_pass",
    auth_logic = "simple_may_will_pass.auth",
    dataflow_graph = "simple_may_will.arcs",
)

policy_check(
    name = "may_will_fail",
    auth_logic = "simple_may_will_fail.auth",
    dataflow_graph = "simple_may_will.arcs",
    expect_failure = True,
)
