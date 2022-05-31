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

package(
    default_visibility = [
        "//src/ir:__pkg__",
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
)

cc_library(
    name = "ir_parser",
    srcs = ["ir_parser.cc"],
    hdrs = ["ir_parser.h"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/types",
        "//src/parser/ir:ir_parser_generator",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "ir_parser_test",
    srcs = ["ir_parser_test.cc"],
    copts = ["-fexceptions"],
    deps = [
        ":ir_parser",
        "//src/common/testing:gtest",
        "//src/ir:ir_printer",
    ],
)
