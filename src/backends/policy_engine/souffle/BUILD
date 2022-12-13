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
        "//src/ir/auth_logic:universe_relation_insertion",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status:statusor",
    ],
)

raksha_policy_verifier_library(
    name = "datalog_multiple_policy_verifier_pass",
    additional_dl_files = ["//src/analysis/souffle/tests:test_output.dl"],
    policies = [
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_1.auth",
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_2.auth",
    ],
    policy_verifier_interfaces = ["//src/analysis/souffle/tests:test_policy_verifier_interface.dl"],
)

raksha_policy_verifier_library(
    name = "datalog_multiple_policy_verifier_fail",
    additional_dl_files = ["//src/analysis/souffle/tests:test_output.dl"],
    policies = [
        "//src/backends/policy_engine/souffle/testdata:multiple_passing_policy_1.auth",
        "//src/backends/policy_engine/souffle/testdata:empty_policy.auth",
    ],
    policy_verifier_interfaces = ["//src/analysis/souffle/tests:test_policy_verifier_interface.dl"],
)

raksha_policy_verifier_library(
    name = "datalog_simple_policy_verifier_pass",
    additional_dl_files = ["//src/analysis/souffle/tests:test_output.dl"],
    policies = ["//src/backends/policy_engine/souffle/testdata:simple_passing_policy.auth"],
    policy_verifier_interfaces = ["//src/analysis/souffle/tests:test_policy_verifier_interface.dl"],
)

raksha_policy_verifier_library(
    name = "datalog_simple_policy_verifier_fail",
    additional_dl_files = ["//src/analysis/souffle/tests:test_output.dl"],
    policies = ["//src/backends/policy_engine/souffle/testdata:simple_failing_policy.auth"],
    policy_verifier_interfaces = ["//src/analysis/souffle/tests:test_policy_verifier_interface.dl"],
)

cc_binary(
    name = "check_policy_compliance",
    srcs = ["check_policy_compliance.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":datalog_multiple_policy_verifier_fail",
        ":datalog_multiple_policy_verifier_pass",
        ":datalog_simple_policy_verifier_fail",
        ":datalog_simple_policy_verifier_pass",
        ":souffle_policy_checker",
        "//src/analysis/souffle/examples/multimic:check_multimic_pass",
        "//src/analysis/souffle/examples/multimic:check_multimic_userc_tag_fail",
        "//src/analysis/souffle/examples/simple:ok_claim_propagates_check",
        "//src/analysis/souffle/examples/simple_canSayDowngrades:delegation_granted_downgrades",
        "//src/analysis/souffle/examples/simple_canSayDowngrades:delegation_not_granted_by_non_owner",
        "//src/analysis/souffle/examples/simple_canSayDowngrades:delegation_not_granted_downgrades",
        "//src/analysis/souffle/examples/simple_canSayHasTag:delegation_granted",
        "//src/analysis/souffle/examples/simple_canSayHasTag:delegation_not_granted",
        "//src/analysis/souffle/examples/simple_may_will:may_will_fail",
        "//src/analysis/souffle/examples/simple_may_will:may_will_pass",
        "//src/backends/policy_engine:auth_logic_policy",
        "//src/backends/policy_engine:catchall_policy_rule_policy",
        "//src/backends/policy_engine:dp_parameter_policy",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine/souffle/dp_testdata/delta_1:dp_policy_verifier_delta_1_with_auth_logic_lib",
        "//src/backends/policy_engine/souffle/dp_testdata/delta_2:dp_policy_verifier_delta_2_with_auth_logic_lib",
        "//src/backends/policy_engine/souffle/dp_testdata/epsilon_2:dp_policy_verifier_epsilon_2_with_auth_logic_lib",
        "//src/backends/policy_engine/souffle/dp_testdata/epsilon_4:dp_policy_verifier_epsilon_4_with_auth_logic_lib",
        "//src/ir:proto_to_ir",
        "//src/parser/ir:ir_parser",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status:statusor",
    ],
)

[sh_test(
    name = "%s" % test_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "sql_policy_analysis",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:test.ir)",
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
        "%s" % policy_engine,
    ],
    data = [
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
        "//src/backends/policy_engine/souffle/testdata:test.ir",
    ],
) for test_name, test_result, policy_engine in [
    ("check_multiple_policy_compliance_pass_test", "pass", "datalog_multiple_policy_verifier_pass_cxx"),
    ("check_multiple_policy_compliance_fail_test", "fail", "datalog_multiple_policy_verifier_fail_cxx"),
    ("check_simple_policy_compliance_pass_test", "pass", "datalog_simple_policy_verifier_pass_cxx"),
    ("check_simple_policy_compliance_fail_test", "fail", "datalog_simple_policy_verifier_fail_cxx"),
]]

[sh_test(
    name = "check_policy_compliance_%s_test" % ir_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "sql_policy_analysis",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:%s.ir)" % ir_name,
        "$(location //src/backends/policy_engine/souffle/testdata:arcsjs_policy_rules.txt)",
    ],
    data = [
        "//src/backends/policy_engine/souffle/testdata:%s.ir" % ir_name,
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:arcsjs_policy_rules.txt",
    ],
) for ir_name, test_result in [
    ("simple_passing_arcsjs", "pass"),
    ("simple_failing_arcsjs", "fail"),
    ("check_not_passing_arcsjs", "pass"),
    ("check_not_failing_arcsjs_has_tag", "fail"),
    ("user_action_passing_arcsjs", "pass"),
    ("large_sample_passing_arcsjs", "pass"),
    ("user_action_failing_arcsjs_modified_after_consent", "fail"),
    ("user_action_failing_arcsjs_no_consent", "fail"),
    ("user_action_failing_arcsjs_no_intent", "fail"),
    ("user_action_failing_arcsjs_partial_downgrade", "fail"),
    ("user_action_failing_arcsjs_wrong_downgrade", "fail"),
]]

[sh_test(
    name = "check_policy_compliance_%s_test_sql" % test_result,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "sql_policy_analysis",
        "ir",
        "$(location //src/backends/policy_engine/souffle/testdata:%s.ir)" % ir_name,
        "$(location //src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt)",
    ],
    data = [
        "//src/backends/policy_engine/souffle/testdata:%s.ir" % ir_name,
        ":check_policy_compliance",
        "//src/backends/policy_engine/souffle/testdata:sql_policy_rules.txt",
    ],
) for ir_name, test_result in [
    ("simple_passing_sql", "pass"),
    ("simple_failing_sql", "fail"),
]]

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

sh_test(
    name = "check_policy_compliance_proto_pass_test",
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        "pass",
        "sql_policy_analysis",
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

[sh_test(
    name = "check_dp_policy_compliance_epsilon_2_%s_test" % ir_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "dp_analysis",
        "$(location //src/backends/policy_engine/souffle/dp_testdata/epsilon_2:%s.ir)" % ir_name,
        "2",
        "1",
        "dp_policy_verifier_cxx",
    ],
    data = [
        "//src/backends/policy_engine/souffle/dp_testdata/epsilon_2:%s.ir" % ir_name,
        ":check_policy_compliance",
    ],
) for ir_name, test_result in [
    ("1q_nopm_failing", "fail"),
    ("1q_passing", "pass"),
    ("1q_no_aggregation_failing", "fail"),
    ("2q_passing", "pass"),
    ("1q_no_aggregation_passing", "pass"),
    ("1q_failing", "fail"),
    ("1q_join_failing", "fail"),
    ("1q_join_passing_gb", "pass"),
    ("1q_join_passing_kappa", "pass"),
    ("2q_failing", "fail"),
]]

[sh_test(
    name = "check_dp_policy_compliance_epsilon_4_%s_test" % ir_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "dp_analysis",
        "$(location //src/backends/policy_engine/souffle/dp_testdata/epsilon_4:%s.ir)" % ir_name,
        "4",
        "1",
        "dp_policy_verifier_cxx",
    ],
    data = [
        "//src/backends/policy_engine/souffle/dp_testdata/epsilon_4:%s.ir" % ir_name,
        ":check_policy_compliance",
    ],
) for ir_name, test_result in [
    ("1q_passing", "pass"),
    ("2q_passing", "pass"),
    ("1q_failing", "fail"),
    ("2q_failing", "fail"),
]]

[sh_test(
    name = "check_dp_policy_compliance_delta_1_%s_test" % ir_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "dp_analysis",
        "$(location //src/backends/policy_engine/souffle/dp_testdata/delta_1:%s.ir)" % ir_name,
        "4",
        "1",
        "dp_policy_verifier_cxx",
    ],
    data = [
        "//src/backends/policy_engine/souffle/dp_testdata/delta_1:%s.ir" % ir_name,
        ":check_policy_compliance",
    ],
) for ir_name, test_result in [
    ("1q_passing", "pass"),
    ("2q_passing", "pass"),
    ("1q_failing", "fail"),
    ("2q_failing", "fail"),
]]

[sh_test(
    name = "check_dp_policy_compliance_delta_2_%s_test" % ir_name,
    srcs = ["check_policy_compliance_test.sh"],
    args = [
        "$(location :check_policy_compliance)",
        test_result,
        "dp_analysis",
        "$(location //src/backends/policy_engine/souffle/dp_testdata/delta_2:%s.ir)" % ir_name,
        "4",
        "2",
        "dp_policy_verifier_cxx",
    ],
    data = [
        "//src/backends/policy_engine/souffle/dp_testdata/delta_2:%s.ir" % ir_name,
        ":check_policy_compliance",
    ],
) for ir_name, test_result in [
    ("1q_passing", "pass"),
    ("2q_passing", "pass"),
    ("1q_failing", "fail"),
    ("2q_failing", "fail"),
]]

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
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir:ssa_names",
        "//src/ir:value",
        "//src/ir:value_string_converter",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:float_attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/datalog:raksha_relation_interface",
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
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:float_attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
        "@com_google_absl//absl/strings",
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
        "@com_google_absl//absl/status:statusor",
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
        "//src/ir/attributes:attribute",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
        "@com_google_absl//absl/strings",
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
        ":utils",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:policy_checker",
        "//src/common/utils:filesystem",
        "//src/ir:module",
        "@com_google_absl//absl/strings",
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
        "//src/parser/ir:ir_parser",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = ["utils.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
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
        "@com_google_absl//absl/status:statusor",
    ],
)

exports_files(["check_policy_compliance_test.sh"])
