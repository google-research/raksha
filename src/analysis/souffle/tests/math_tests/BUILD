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
    default_applicable_licenses = ["//third_party/raksha:license"],
    features = ["layering_check"],
    licenses = ["notice"],
)

output_rule_diff_test(
    name = "math_unit_test",
    expected_contents_csv = ":expectedResult.csv",
    input_files = glob(["*.facts"]),
    output_fact = "logResult",
    test_bin = ["//src/analysis/souffle:math_test"],
)
