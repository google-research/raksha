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
load(
    "//src/analysis/souffle:dl_file_lists.bzl",
    "core_dl_files",
    "core_dl_files_except_taint",
    "export_dl_files_local",
)
load(
    "//build_defs:raksha_policy_verifier.bzl",
    "raksha_policy_verifier_library",
)

package(
    default_applicable_licenses = ["//third_party/raksha:license"],
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "logfunctors",
    srcs = [
        "functors.cc",
    ],
    hdrs = ["functors.h"],
    copts = [
        "-fopenmp",
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    local_defines = ["RAM_DOMAIN_SIZE=64"],
    deps = [
        "//src/common/logging",
        "@souffle//:souffle_include_lib",
    ],
)

cc_test(
    name = "functors_test",
    srcs = [
        "functors_test.cc",
    ],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    local_defines = ["RAM_DOMAIN_SIZE=64"],
    deps = [
        ":logfunctors",
        "//src/common/testing:gtest",
        "@souffle//:souffle_include_lib",
    ],
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
    included_dl_scripts = core_dl_files_except_taint,
)

gen_souffle_cxx_code(
    name = "tag_check_and_expectation_for_test",
    src = "tag_check_and_expectation_interface.dl",
    for_test = True,
    included_dl_scripts = core_dl_files,
)

souffle_cc_binary(
    name = "tag_check_and_expectation_test",
    testonly = True,
    src = ":tag_check_and_expectation_for_test",
)

sql_dl_files = [
    "sql_output.dl",
    "tag_transforms.dl",
]

gen_souffle_cxx_code(
    name = "sql_verifier_tag_unit_test_for_test",
    src = "sql_verifier_tag_unit_test_interface.dl",
    for_test = True,
    included_dl_scripts = core_dl_files + sql_dl_files,
)

souffle_cc_binary(
    name = "sql_verifier_tag_unit_test_test",
    testonly = True,
    src = ":sql_verifier_tag_unit_test_for_test",
)

gen_souffle_cxx_code(
    name = "sensitivity_analysis_test_for_test",
    src = "sensitivity_analysis_unit_test_interface.dl",
    for_test = True,
    included_dl_scripts = core_dl_files + sql_dl_files + [
        "sensitivity_analysis.dl",
    ],
)

souffle_cc_binary(
    name = "sensitivity_analysis_test",
    testonly = True,
    src = ":sensitivity_analysis_test_for_test",
)

gen_souffle_cxx_code(
    name = "sql_verifier_unit_test_code",
    src = "sql_verifier_unit_test_interface.dl",
    included_dl_scripts = core_dl_files + sql_dl_files,
)

souffle_cc_binary(
    name = "sql_verifier_unit_test",
    src = ":sql_verifier_unit_test_code",
)

gen_souffle_cxx_code(
    name = "math_test_for_test",
    src = "math_unit_test_interface.dl",
    for_test = True,
    included_dl_scripts = core_dl_files + sql_dl_files + [
        "math.dl",
    ],
)

souffle_cc_binary(
    name = "math_test",
    testonly = True,
    src = ":math_test_for_test",
    additional_deps = ["logfunctors"],
)

# TODO(#728): This datalog_policy_verifier is very entangled and is doing too many things. We should
# untangle it and delete it.
raksha_policy_verifier_library(
    name = "datalog_policy_verifier",
    additional_dl_files = [
        ":epsilon_analysis.dl",
        ":sensitivity_analysis.dl",
        ":math.dl",
    ],
    additional_souffle_cc_lib_dependencies = ["logfunctors"],
    policies = ["//src/backends/policy_engine/souffle/testdata:empty_policy.auth"],
    policy_verifier_interfaces = [":policy_verifier_interface.dl"],
)

# This is a policy verifier focused on the exfiltration of secret columns in the SQL verification
# engine specifically.
raksha_policy_verifier_library(
    name = "sql_policy_verifier",
    policies = ["//src/backends/policy_engine/souffle/testdata:empty_policy.auth"],
    policy_verifier_interfaces = [":sql_policy_verifier_interface.dl"],
)

# This is a policy verifier focused on the propagation of differential privacy parameter info.
raksha_policy_verifier_library(
    name = "dp_policy_verifier",
    additional_dl_files = [
        ":epsilon_analysis.dl",
        ":sensitivity_analysis.dl",
        ":math.dl",
    ],
    policies = ["//src/backends/policy_engine/souffle/testdata:empty_policy.auth"],
    policy_verifier_interfaces = [":dp_policy_verifier_interface.dl"],
)

souffle_cc_library(
    name = "taint_dl",
    src = ":taint_cxx",
)

exports_files(export_dl_files_local)
