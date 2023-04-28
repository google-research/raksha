#-------------------------------------------------------------------------------
# Copyright 2023 Google LLC
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
    default_applicable_licenses = ["//third_party/raksha:license"],
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "abstract_ifc_tags",
    srcs = ["abstract_ifc_tags.cc"],
    hdrs = ["abstract_ifc_tags.h"],
    deps = [
        ":inference_rules",
        "//src/common/utils:intrusive_ptr",
        "//src/common/utils:ref_counted",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:btree",
    ],
)

cc_test(
    name = "abstract_ifc_tags_test",
    srcs = ["abstract_ifc_tags_test.cc"],
    deps = [
        ":abstract_ifc_tags",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "abstract_semantics",
    srcs = ["abstract_semantics.cc"],
    hdrs = ["abstract_semantics.h"],
    deps = [
        ":abstract_ifc_tags",
        ":inference_rules",
        "//src/analysis/common:module_graph",
        "//src/common/utils:fold",
        "//src/common/utils:overloaded",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_test(
    name = "abstract_semantics_test",
    srcs = ["abstract_semantics_test.cc"],
    deps = [
        ":abstract_ifc_tags",
        ":abstract_semantics",
        ":inference_rule",
        ":inference_rules",
        ":inference_rules_builder",
        "//src/common/testing:gtest",
        "//src/common/utils:map_iter",
        "//src/ir:module",
        "//src/ir:storage",
        "//src/ir:value",
        "//src/ir/types",
        "//src/parser/ir:ir_parser",
    ],
)

cc_library(
    name = "inference_rule",
    hdrs = ["inference_rule.h"],
)

cc_test(
    name = "inference_rule_test",
    srcs = ["inference_rule_test.cc"],
    deps = [
        ":inference_rule",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "inference_rules",
    hdrs = [
        "inference_rules.h",
    ],
    deps = [
        ":inference_rule",
        "//src/ir:module",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "inference_rules_test",
    srcs = ["inference_rules_test.cc"],
    deps = [
        ":inference_rule",
        ":inference_rules",
        "//src/common/testing:gtest",
        "//src/ir:module",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "inference_rules_builder",
    hdrs = [
        "inference_rules_builder.h",
    ],
    deps = [
        ":inference_rules",
        "//src/ir:module",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "inference_rules_builder_test",
    srcs = ["inference_rules_builder_test.cc"],
    deps = [
        ":inference_rules",
        ":inference_rules_builder",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
    ],
)
