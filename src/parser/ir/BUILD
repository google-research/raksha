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
load("//build_defs:antlr.bzl", "antlr4_cc_combined")
load("//third_party/googlefuzztest/build_defs:cc_fuzztest_grammar_library.bzl", "cc_fuzztest_grammar_library")

package(
    default_visibility = [
        "//intelligence/federated_perception:__subpackages__",
        "//src:__subpackages__",
    ],
    features = ["layering_check"],
    licenses = ["notice"],
)

exports_files(
    ["Ir.g4"],
)

#antlr
antlr4_cc_combined(
    name = "ir_parser_generator",
    src = "Ir.g4",
    listener = False,
    visitor = True,
)

cc_fuzztest_grammar_library(
    name = "ir_grammar",
    srcs = ["Ir.g4"],
)

cc_library(
    name = "ir_parser",
    srcs = ["ir_parser.cc"],
    hdrs = ["ir_parser.h"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ir_parser_generator",
        "//src/common/utils:fold",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/common/utils:types",
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir:ssa_names",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:float_attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "@com_google_absl//absl/log:check",
    ],
)

cc_test(
    name = "ir_parser_test",
    srcs = ["ir_parser_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ir_grammar",
        ":ir_parser",
        "//src/common/testing:gtest",
        "//src/ir:ir_printer",
        "@fuzztest//fuzztest",
    ],
)

cc_binary(
    name = "ir_to_operation_facts",
    srcs = ["ir_to_operation_facts.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ir_parser",
        "//src/backends/policy_engine/souffle:datalog_lowering_visitor",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/strings",
    ],
)
