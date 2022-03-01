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

licenses(["notice"])

package(default_visibility = ["//src:__subpackages__"])

load("@rules_antlr//antlr:antlr4.bzl", "antlr")

load("//build_defs:antlr.bzl", "authorization_logic_datalog_generator", "antlr_cc_library")

cc_library(
    name = "ast",
    hdrs = [
        "ast.h",
    ],
    visibility = ["//test:__pkg__"],
    deps = [
        "//src/common/logging",
        "//src/ir/datalog:program",
        "//src/utils:overloaded",
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
        "//src/ir/datalog:program",
        "//src/utils:map_iter",
        "//src/utils:move_append",
        "//src/utils:overloaded",
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
        "//src/ir/auth_logic:ast",
        "//src/ir/auth_logic:lowering_ast_datalog",
        "//src/ir/datalog:program",
        "//src/utils:map_iter",
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
        "//src/utils:move_append",
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
        name = "AuthLogic",
        srcs = ["AuthLogic.g4"],
        language = "Cpp",
        listener = True,
        visitor = True,
        package = "",
        
    )
    
cc_library(
        name =  "AuthLogic_cc_parser",
        srcs = ["AuthLogic"],
        copts = ["-fexceptions",],
        linkopts = ["-fexceptions",],
        features = ["-use_header_modules"],
        deps = [
            "AuthLogic",
            "@antlr4_runtimes//:cpp",
        ],
        alwayslink = True,
        linkstatic = 1,
    )

cc_library(
    name = "ast_construction",
    srcs = ["ast_construction.cc"],
    copts = ["-fexceptions",],
    deps = ["@antlr4_runtimes//:cpp",
            "//src/ir/auth_logic:ast",
            "//src/ir/auth_logic:AuthLogic_cc_parser",
            "//src/ir/datalog:program",
            "//src/utils:overloaded",
            "//src/utils:map_iter",
            "//src/utils:move_append",
    ],
    hdrs = ["ast_construction.h"]

)

# cc_binary(
#     name = "generate_auth_datalog_program",
#     srcs = ["generate_auth_datalog_program.cc"],
#     deps = ["@absl//absl/flags:flag",
#             "@absl//absl/flags:parse",
#             "//src/common/logging",
#             "//src/ir/auth_logic:ast_construction",
#             "//src/ir/auth_logic:ast",
#            ],
# )

# authorization_logic_datalog_generator() #default test for canActAs. 