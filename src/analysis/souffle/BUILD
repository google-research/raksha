#-------------------------------------------------------------------------------
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

load(
    "//build_defs:souffle.bzl",
    "gen_souffle_cxx_code",
    "souffle_cc_binary",
    "souffle_cc_library",
)

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_test(
    name = "cpp_interface_test",
    srcs = ["cpp_interface_test.cc"],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    linkopts = ["-pthread"],
    deps = [
        ":taint_dl",
        "//src/common/testing:gtest",
        "@souffle//:souffle_include_lib",
    ],
)

gen_souffle_cxx_code(
    name = "taint_cxx",
    src = "taint.dl",
    included_dl_scripts = [
        "attributes.dl",
        "authorization_logic.dl",
        "check_predicate.dl",
        "dataflow_graph.dl",
        "operations.dl",
        "tags.dl",
    ],
)

gen_souffle_cxx_code(
    name = "tag_check_and_expectation_for_test",
    src = "tag_check_and_expectation_interface.dl",
    for_test = True,
    included_dl_scripts = [
        "attributes.dl",
        "authorization_logic.dl",
        "check_predicate.dl",
        "dataflow_graph.dl",
        "operations.dl",
        "tags.dl",
        "taint.dl",
    ],
)

souffle_cc_binary(
    name = "tag_check_and_expectation_test",
    testonly = True,
    src = ":tag_check_and_expectation_for_test",
)

gen_souffle_cxx_code(
    name = "sql_verifier_tag_unit_test_for_test",
    src = "sql_verifier_tag_unit_test_interface.dl",
    for_test = True,
    included_dl_scripts = [
        "attributes.dl",
        "authorization_logic.dl",
        "check_predicate.dl",
        "dataflow_graph.dl",
        "operations.dl",
        "tag_transforms.dl",
        "tags.dl",
        "taint.dl",
    ],
)

souffle_cc_binary(
    name = "sql_verifier_tag_unit_test_test",
    testonly = True,
    src = ":sql_verifier_tag_unit_test_for_test",
)

souffle_cc_library(
    name = "taint_dl",
    src = ":taint_cxx",
)

exports_files([
    "attributes.dl",
    "authorization_logic.dl",
    "check_predicate.dl",
    "dataflow_graph.dl",
    "fact_test_helper.dl",
    "operations.dl",
    "tags.dl",
    "taint.dl",
    "may_will.dl",
])
