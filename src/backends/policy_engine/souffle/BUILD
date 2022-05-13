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

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
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
        "//src/frontends/sql:decoder_context",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir/attributes:attribute",
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
        "//src/common/testing:gtest",
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
    deps = [
        "//src/analysis/souffle:datalog_policy_verifier_lib",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:policy_checker",
        "//src/ir:module",
        "@absl//absl/strings",
        "@souffle//:souffle_include_lib",
    ],
)

cc_test(
    name = "souffle_policy_checker_test",
    srcs = ["souffle_policy_checker_test.cc"],
    deps = [
        ":souffle_policy_checker",
        "//src/common/logging",
        "//src/common/testing:gtest",
    ],
)
