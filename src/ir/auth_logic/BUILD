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
load("@rules_antlr//antlr:antlr4.bzl", "antlr")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "ast",
    hdrs = [
        "ast.h",
    ],
    visibility = ["//test:__pkg__"],
    deps = [
        "//src/common/logging",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@absl//absl/hash",
        "@absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "lowering_ast_datalog",
    srcs = ["lowering_ast_datalog.cc"],
    hdrs = [
        "lowering_ast_datalog.h",
    ],
    visibility = ["//test:__pkg__"],
    deps = [
        ":ast",
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@absl//absl/algorithm:container",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "souffle_emitter",
    srcs = ["souffle_emitter_build_stub.cc"],
    hdrs = [
        "souffle_emitter.h",
    ],
    deps = [
        "//src/common/utils:map_iter",
        "//src/ir/auth_logic:ast",
        "//src/ir/auth_logic:lowering_ast_datalog",
        "//src/ir/datalog:program",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "souffle_emitter_test",
    srcs = ["souffle_emitter_test.cc"],
    deps = [
        ":ast",
        ":lowering_ast_datalog",
        ":souffle_emitter",
        "//src/common/testing:gtest",
        "//src/ir/datalog:program",
    ],
)

cc_test(
    name = "ast_test",
    srcs = ["ast_test.cc"],
    deps = [
        ":ast",
        "//src/common/testing:gtest",
    ],
)

#antlr

antlr(
    name = "auth_logic_antlr",
    srcs = ["//rust/tools/authorization-logic:authlogic_grammar"],
    language = "Cpp",
    listener = False,
    package = "",
    visitor = True,
)

cc_library(
    name = "auth_logic_cc_parser",
    srcs = [":auth_logic_antlr"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    linkopts = ["-fexceptions"],
    deps = [
        "auth_logic_antlr",
        "@antlr4_runtimes//:antlr_runtime_build",
    ],
)
