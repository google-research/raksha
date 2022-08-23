#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-----------------------------------------------------------------------------
"""Lists of dl files that can be shared across multiple BUILD files."""

core_dl_files_except_taint_local = [
    "attributes.dl",
    "authorization_logic.dl",
    "check_predicate.dl",
    "dataflow_graph.dl",
    "operations.dl",
    "tags.dl",
]

core_dl_files_local = core_dl_files_except_taint_local + ["taint.dl"]

core_dl_files_plus_sql_output_local = core_dl_files_local + ["sql_output.dl"]

core_dl_files_plus_fact_test_helper_local = core_dl_files_local + ["fact_test_helper.dl"]

policy_verifier_include_dl_files_local = core_dl_files_plus_fact_test_helper_local + [
    "may_will.dl",
    "sql_output.dl",
    "tag_transforms.dl",
    "epsilon_analysis.dl",
    "sensitivity_analysis.dl",
]

policy_verifier_dl_files_local = policy_verifier_include_dl_files_local + [
    "policy_verifier_interface.dl",
]

absolute_target_path = "//src/analysis/souffle:"

core_dl_files_except_taint = [
    absolute_target_path + target
    for target in core_dl_files_except_taint_local
]

core_dl_files = [
    absolute_target_path + target
    for target in core_dl_files_local
]

core_dl_files_plus_fact_test_helper = [
    absolute_target_path + target
    for target in core_dl_files_plus_fact_test_helper_local
]

policy_verifier_include_dl_files = [
    absolute_target_path + target
    for target in policy_verifier_include_dl_files_local
]
