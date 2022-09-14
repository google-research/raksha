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
    "//build_defs:test_helpers.bzl",
    "convert_ir_to_is_operation_facts",
    "output_rule_diff_test",
)

convert_ir_to_is_operation_facts(
    name = "multi_column_grouping_join_conv",
    ir_file = ":multi_column_grouping_join.ir",
    out_file = "isOperation.facts",
)

output_rule_diff_test(
    name = "grouping_join_unit_test",
    expected_contents_csv = ":expectedResult.csv",
    input_files = [":isOperation.facts"],
    output_fact = "isGroupingJoin",
    test_bin = ["//src/analysis/souffle:grouping_join_unit_test"],
)
