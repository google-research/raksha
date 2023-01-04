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
    name = "module_graph",
    srcs = ["module_graph.cc"],
    hdrs = ["module_graph.h"],
    deps = [
        "//src/common/utils:iterator_adapter",
        "//src/common/utils:ranges",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "//src/ir:value",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/log:die_if_null",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "module_graph_test",
    srcs = ["module_graph_test.cc"],
    deps = [
        ":module_graph",
        "//src/common/testing:gtest",
        "//src/common/utils:iterator_adapter",
        "//src/common/utils:overloaded",
        "//src/ir:ir_printer",
        "//src/ir:module",
        "//src/ir:ssa_names",
        "//src/ir:value",
        "//src/ir:value_string_converter",
        "//src/parser/ir:ir_parser",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "module_fixpoint_iterator",
    hdrs = ["module_fixpoint_iterator.h"],
    deps = [
        ":module_graph",
        "//src/common/utils:map_iter",
        "//src/common/utils:overloaded",
        "//src/ir:module",
        "//src/ir:value",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_test(
    name = "module_fixpoint_iterator_test",
    srcs = ["module_fixpoint_iterator_test.cc"],
    deps = [
        ":module_fixpoint_iterator",
        "//src/common/testing:gtest",
        "//src/common/utils:fold",
        "//src/ir:ssa_names",
        "//src/ir:value_string_converter",
        "//src/parser/ir:ir_parser",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:btree",
    ],
)

cc_library(
    name = "worklist_fixpoint_iterator",
    hdrs = ["worklist_fixpoint_iterator.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log:check",
    ],
)

cc_test(
    name = "worklist_fixpoint_iterator_test",
    srcs = ["worklist_fixpoint_iterator_test.cc"],
    deps = [
        ":worklist_fixpoint_iterator",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)
