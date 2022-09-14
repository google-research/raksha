#-------------------------------------------------------------------------------
# Copyright 2022 Google LLC
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
#------------------------------------------------------------------------------

load("//build_defs:test_helpers.bzl", "convert_ir_to_is_operation_facts")

convert_ir_to_is_operation_facts(
    name = "dp_ir_test_convert",
    ir_file = ":2q_passing.ir",
    out_file = "actual.facts",
)

sh_test(
    name = "dp_ir_test",
    srcs = ["//src/analysis/souffle/tests/arcs_fact_tests:diff_wrapper.sh"],
    args = [
        "$(location :expected.facts)",
        "$(location :actual.facts)",
    ],
    data = [
        ":actual.facts",
        ":expected.facts",
    ],
)
