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

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "ast",
    hdrs = [
        "ast.h",
        "auth_logic_ast_visitor.h",
    ],
    visibility = ["//visibility:private"],
    deps = [
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/common/utils:types",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "auth_logic_ast_traversing_visitor",
    hdrs = [
        "auth_logic_ast_traversing_visitor.h",
        "auth_logic_ast_visitor.h",
    ],
    deps = [
        ":ast",
        "//src/common/logging",
        "//src/common/utils:fold",
        "//src/common/utils:overloaded",
        "//src/common/utils:types",
        "//src/ir/datalog:program",
    ],
)

cc_library(
    name = "declaration_environment",
    srcs = ["declaration_environment.cc"],
    hdrs = ["declaration_environment.h"],
    deps = [
        ":ast",
        ":auth_logic_ast_traversing_visitor",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "declaration_environment_test",
    srcs = ["declaration_environment_test.cc"],
    deps = [
        ":ast",
        ":declaration_environment",
        "//src/common/testing:gtest",
        "//src/ir/datalog:program",
    ],
)

cc_library(
    name = "is_name_constant",
    srcs = ["is_name_constant.cc"],
    hdrs = ["is_name_constant.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_test(
    name = "is_name_constant_test",
    srcs = [
        "is_name_constant_test.cc",
    ],
    deps = [
        ":is_name_constant",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "type_environment",
    srcs = ["type_environment.cc"],
    hdrs = [
        "is_name_constant.h",
        "type_environment.h",
    ],
    deps = [
        ":ast",
        ":auth_logic_ast_traversing_visitor",
        ":declaration_environment",
        ":is_name_constant",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "type_environment_test",
    srcs = ["type_environment_test.cc"],
    deps = [
        ":ast",
        ":declaration_environment",
        ":type_environment",
        "//src/common/testing:gtest",
        "//src/ir/datalog:program",
    ],
)

cc_library(
    name = "universe_relation_insertion",
    srcs = ["universe_relation_insertion.cc"],
    hdrs = [
        "universe_relation_insertion.h",
    ],
    visibility = ["//src/backends/policy_engine/souffle:__subpackages__"],
    deps = [
        ":ast",
        ":auth_logic_ast_traversing_visitor",
        ":declaration_environment",
        ":is_name_constant",
        ":type_environment",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_library(
    name = "lowering_ast_datalog",
    srcs = ["lowering_ast_datalog.cc"],
    hdrs = [
        "lowering_ast_datalog.h",
    ],
    visibility = ["//src/backends/policy_engine/souffle:__subpackages__"],
    deps = [
        ":ast",
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "souffle_emitter",
    srcs = ["souffle_emitter_build_stub.cc"],
    hdrs = [
        "souffle_emitter.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/ir/auth_logic:ast",
        "//src/ir/auth_logic:lowering_ast_datalog",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "ast_equality_test",
    srcs = ["ast_equality_test.cc"],
    deps = [
        ":ast",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "universe_relation_insertion_test",
    srcs = ["universe_relation_insertion_test.cc"],
    deps = [
        ":ast",
        ":universe_relation_insertion",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
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

cc_test(
    name = "auth_logic_ast_traversing_visitor_test",
    srcs = ["auth_logic_ast_traversing_visitor_test.cc"],
    deps = [
        ":ast",
        ":auth_logic_ast_traversing_visitor",
        "//src/common/testing:gtest",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/container:btree",
    ],
)

cc_library(
    name = "ast_construction",
    srcs = ["ast_construction.cc"],
    hdrs = ["ast_construction.h"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/auth_logic:ast",
        "//src/ir/datalog:program",
        "//src/policy/auth_logic:auth_logic_cc_generator",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "ast_string",
    srcs = ["ast_string.cc"],
    hdrs = ["ast_string.h"],
    deps = [
        ":ast",
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "ast_string_test",
    srcs = ["ast_string_test.cc"],
    deps = [
        ":ast",
        ":ast_construction",
        ":ast_string",
        "//src/common/testing:gtest",
        "//src/ir/datalog:program",
    ],
)

cc_test(
    name = "ast_construction_query_test",
    srcs = ["ast_construction_query_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ast",
        ":ast_construction",
        ":ast_string",
        "//src/common/testing:gtest",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "ast_construction_says_assertion_test",
    srcs = ["ast_construction_says_assertion_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ast",
        ":ast_construction",
        ":ast_string",
        "//src/common/testing:gtest",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "ast_construction_relation_declaration_test",
    srcs = ["ast_construction_relation_declaration_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ast",
        ":ast_construction",
        ":ast_string",
        "//src/common/testing:gtest",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir/datalog:program",
        "@com_google_absl//absl/strings",
    ],
)
