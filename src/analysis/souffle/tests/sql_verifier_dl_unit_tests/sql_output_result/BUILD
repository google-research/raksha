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
#-------------------------------------------------------------------------------
load(
    "//build_defs:test_helpers.bzl",
    "output_rule_diff_test",
)

package(
    features = ["layering_check"],
    licenses = ["notice"],
)

output_rule_diff_test(
    name = "check_sql_output_unit_test",
    expected_contents_csv = ":expected.csv",
    input_files = glob(["*.facts"]),
    output_fact = "sqlOutputResult",
    test_bin = "//src/analysis/souffle:sql_verifier_unit_test",
)
