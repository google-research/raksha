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
# Policy checker that is based on computing fix points using chaotic iterations.

# load(
#     "//third_party/raksha/build_defs:raksha_policy_verifier.bzl",
#     "raksha_policy_verifier_library",
# )
package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "chaotic_policy_checker",
    srcs = ["chaotic_policy_checker.cc"],
    hdrs = ["chaotic_policy_checker.h"],
    deps = [
        ":sql_policy_rules",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:policy_checker",
        "//src/frontends/sql/ops:merge_op",
        "//src/frontends/sql/ops:sql_op",
        "//src/frontends/sql/ops:sql_output_op",
        "//src/frontends/sql/ops:tag_transform_op",
        "//src/ir:ir_printer",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "sql_policy_rules",
    srcs = ["sql_policy_rules.cc"],
    hdrs = ["sql_policy_rules.h"],
    deps = [
        "//src/common/utils:map_iter",
        "//third_party/raksha:raksha_sql_ir_cc_proto",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
    ],
)
