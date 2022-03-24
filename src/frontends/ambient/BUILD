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
load("//build_defs:raksha.bzl", "raksha_policy_check")
load(
    "//build_defs:souffle.bzl",
    "gen_souffle_cxx_code",
    "souffle_cc_binary",
    "souffle_cc_library",
)

licenses(["notice"])

raksha_policy_check(
    name = "multimic",
    src = "multimic.raksha",
)

souffle_cc_library(
    name = "policy_checker_dl",
    src = ":policy_checker_cxx",
)

gen_souffle_cxx_code(
    name = "policy_checker_cxx",
    src = ":multimic_datalog",
    included_dl_scripts = [
        "//src/analysis/souffle:attributes.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:check_predicate.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:operations.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:may_will.dl",
    ],
)

cc_library(
    name = "policy_checker",
    srcs = ["policy_checker.cc"],
    hdrs = ["policy_checker.h"],
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
        ":policy_checker_dl",
        "//src/common/logging",
        "//src/common/utils:ranges",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
        "@souffle//:souffle_include_lib",
    ],
)

cc_test(
    name = "policy_checker_test",
    srcs = ["policy_checker_test.cc"],
    deps = [
        ":policy_checker",
        "//src/common/logging",
        "//src/common/testing:gtest",
    ],
)
