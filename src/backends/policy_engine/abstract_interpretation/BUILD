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
package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "abstract_interpretation_policy_checker",
    srcs = ["abstract_interpretation_policy_checker.cc"],
    hdrs = ["abstract_interpretation_policy_checker.h"],
    deps = [
        "//src/analysis/common:module_fixpoint_iterator",
        "//src/analysis/taint:abstract_semantics",
        "//src/analysis/taint:inference_rules",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:policy_checker",
        "//src/ir:module",
    ],
)

cc_test(
    name = "abstract_interpretation_policy_checker_test",
    srcs = ["abstract_interpretation_policy_checker_test.cc"],
    deps = [
        ":abstract_interpretation_policy_checker",
        "//src/analysis/taint:abstract_ifc_tags",
        "//src/analysis/taint:inference_rules",
        "//src/analysis/taint:inference_rules_builder",
        "//src/backends/policy_engine:sql_policy_rule_policy",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/common/utils:ranges",
        "//src/frontends/sql:decode_sql_policy_rules",
        "//src/frontends/sql:sql_ir_cc_proto",
        "//src/frontends/sql/ops:op_traits",
        "//src/ir:value_string_converter",
        "//src/parser/ir:ir_parser",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)
