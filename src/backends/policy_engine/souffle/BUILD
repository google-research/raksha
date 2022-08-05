#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#----------------------------------------------------------------------------
load(
    "//build_defs:raksha_policy_verifier.bzl",
    "policy_library",
    "raksha_policy_verifier_library",
)
load("//build_defs:package_groups.oss.bzl", "frontend_packages")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_binary(
    name = "raksha_policy_datalog_emitter",
    srcs = ["raksha_policy_datalog_emitter.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":utils",
        "//src/ir/auth_logic:ast_construction",
        "//src/ir/auth_logic:lowering_ast_datalog",
        "//src/ir/auth_logic:souffle_emitter",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
        "@absl//absl/status:statusor",
    ],
)

policy_library(
    name = "datalog_policy_verifier",
    auth_logic_files = ["//src/backends/policy_engine/souffle/testdata:empty_policy.auth"],
)

raksha_policy_verifier_library(
    name = "datalog_multiple_policy_verifier_pass",
    policies = [
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_1.auth",
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_2.auth",
    ],
)

raksha_policy_verifier_library(
    name = "datalog_multiple_policy_verifier_fail",
    policies = [
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_1.auth",
        "//src/backends/policy_engine/souffle/testdata:empty_policy.auth",
    ],
)

policy_library(
    name = "datalog_simple_policy_verifier_pass",
    auth_logic_files = ["//src/backends/policy_engine/souffle/testdata:simple_passing_policy.auth"],
)

policy_library(
    name = "datalog_simple_policy_verifier_fail",
    auth_logic_files = ["//src/backends/policy_engine/souffle/testdata:simple_failing_policy.auth"],
)

cc_binary(
    name = "check_policy_compliance",
    srcs = ["check_policy_compliance.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":souffle_policy_checker",
        "//src/backends/policy_engine:auth_logic_policy",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:sql_policy_rule_policy",
        "//src/common/logging",
        "//src/ir:proto_to_ir",
        "//src/parser/ir:ir_parser",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
        "@absl//absl/status:statusor",
    ],
)

sh_test(
    name = "check_multiple_policy_compliance_pass_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "pass",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "datalog_multiple_policy_verifier_pass_datalog_cxx",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_multiple_policy_compliance_fail_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "fail",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "datalog_multiple_policy_verifier_fail_datalog_cxx",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_simple_policy_compliance_pass_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "pass",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "datalog_simple_policy_verifier_pass_datalog_cxx",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_simple_policy_compliance_fail_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "fail",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "datalog_simple_policy_verifier_fail_datalog_cxx",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_pass_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "pass",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple_passing_sql.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple_passing_sql.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_proto_pass_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "pass",
        "proto",
        "$(location //src/backends/policy_engine/souffle/testdata:simple_passing_sql.json)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple_passing_sql.json",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_fail_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "fail",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:simple_failing_sql.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:simple_failing_sql.ir",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

sh_test(
    name = "check_policy_compliance_badargs_test",
    srcs = ["check_policy_compliance_errors.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
)

cc_library(
    name = "datalog_lowering_visitor",
    srcs = ["datalog_lowering_visitor.cc"],
    hdrs = [
        "datalog_lowering_visitor.h",
    ],
    deps = [
        ":raksha_datalog_facts",
        "//src/common/logging",
        "//src/common/utils:fold",
        "//src/common/utils:types",
        "//src/frontends/sql:decoder_context",
        "//src/ir:ir_printer",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir:ssa_names",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:double_attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/datalog:input_relation_fact",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
    ],
)

cc_test(
    name = "datalog_lowering_visitor_test",
    srcs = ["datalog_lowering_visitor_test.cc"],
    deps = [
        ":datalog_lowering_visitor",
        "//src/common/testing:gtest",
        "//src/ir:block_builder",
        "//src/ir:module",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "raksha_datalog_facts",
    srcs = ["raksha_datalog_facts.cc"],
    hdrs = ["raksha_datalog_facts.h"],
    deps = [
        ":utils",
        "//src/common/logging",
        "//src/ir/datalog:input_relation_fact",
        "//src/ir/datalog:raksha_relation_interface",
        "@absl//absl/status:statusor",
    ],
)

cc_test(
    name = "raksha_datalog_facts_test",
    srcs = ["raksha_datalog_facts_test.cc"],
    deps = [
        ":raksha_datalog_facts",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/common/utils:filesystem",
        "//src/common/utils/test:utils",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "souffle_policy_checker",
    srcs = ["souffle_policy_checker.cc"],
    hdrs = ["souffle_policy_checker.h"],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    linkopts = ["-pthread"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":datalog_lowering_visitor",
        ":datalog_multiple_policy_verifier_fail",
        ":datalog_multiple_policy_verifier_pass",
        ":datalog_policy_verifier",
        ":datalog_simple_policy_verifier_fail",
        ":datalog_simple_policy_verifier_pass",
        ":utils",
        "//src/analysis/souffle:datalog_policy_verifier_lib",
        "//src/analysis/souffle:dp_policy_verifier_lib",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:policy_checker",
        "//src/common/utils:filesystem",
        "//src/ir:module",
        "@absl//absl/container:inlined_vector",
        "@absl//absl/strings",
        "@souffle//:souffle_include_lib",
    ],
)

cc_test(
    name = "souffle_policy_checker_test",
    srcs = ["souffle_policy_checker_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":souffle_policy_checker",
        "//src/backends/policy_engine:dp_parameter_policy",
        "//src/backends/policy_engine:sql_policy_rule_policy",
        "//src/common/testing:gtest",
        "//src/frontends/sql/ops:literal_op",
        "//src/frontends/sql/ops:sql_output_op",
        "//src/frontends/sql/ops:tag_transform_op",
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/parser/ir:ir_parser",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    deps = [
        "@absl//absl/status",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "utils_test",
    srcs = ["utils_test.cc"],
    deps = [
        ":utils",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/common/utils:filesystem",
        "@absl//absl/status:statusor",
    ],
)
