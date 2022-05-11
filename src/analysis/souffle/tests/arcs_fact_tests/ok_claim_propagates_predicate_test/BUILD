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
    "run_taint_exec_compare_check_results",
)

package(
    features = ["layering_check"],
    licenses = ["notice"],
)

run_taint_exec_compare_check_results(
    name = "ok_claim_propagates",
    input_files = glob(["*.facts"]),
    test_bin = "//src/analysis/souffle:tag_check_and_expectation_test",
)
