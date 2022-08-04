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
load("//build_defs:package_groups.oss.bzl", "frontend_packages")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "block_builder",
    hdrs = ["block_builder.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":module",
        ":value",
        "//src/ir/types",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "block_builder_test",
    srcs = [
        "block_builder_test.cc",
    ],
    deps = [
        ":block_builder",
        ":ir_context",
        ":ir_printer",
        ":ssa_names",
        "//src/common/testing:gtest",
        "//src/ir/attributes:attribute",
        "//src/ir/types",
    ],
)

cc_library(
    name = "data_decl",
    hdrs = ["data_decl.h"],
    deps = [
        "//src/ir/types",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "data_decl_test",
    srcs = ["data_decl_test.cc"],
    deps = [
        ":data_decl",
        "//src/common/testing:gtest",
        "//src/ir/types",
    ],
)

cc_library(
    name = "ir_context",
    hdrs = ["ir_context.h"],
    visibility = ["//:__subpackages__"] + frontend_packages,
    deps = [
        ":module",
        ":operator",
        "//src/ir/types",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "ir_context_test",
    srcs = ["ir_context_test.cc"],
    deps = [
        ":ir_context",
        "//src/common/testing:gtest",
        "//src/ir/types",
    ],
)

cc_library(
    name = "ir_printer",
    hdrs = ["ir_printer.h"],
    deps = [
        ":ir_traversing_visitor",
        ":module",
        ":ssa_names",
        ":value",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "ir_printer_test",
    srcs = ["ir_printer_test.cc"],
    deps = [
        ":block_builder",
        ":ir_printer",
        ":module",
        ":value",
        "//src/common/testing:gtest",
        "//src/ir/attributes:string_attribute",
        "@absl//absl/container:flat_hash_set",
    ],
)

cc_library(
    name = "ir_traversing_visitor",
    hdrs = ["ir_traversing_visitor.h"],
    deps = [
        ":module",
        "//src/common/utils:fold",
        "//src/common/utils:types",
    ],
)

cc_test(
    name = "ir_traversing_visitor_test",
    srcs = ["ir_traversing_visitor_test.cc"],
    deps = [
        ":block_builder",
        ":ir_traversing_visitor",
        ":module",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "operator",
    hdrs = ["operator.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = ["@absl//absl/strings"],
)

cc_test(
    name = "operator_test",
    srcs = ["operator_test.cc"],
    deps = [
        ":operator",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "operation_test",
    srcs = ["operation_test.cc"],
    deps = [
        ":ir_printer",
        ":module",
        ":ssa_names",
        ":value",
        "//src/common/testing:gtest",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "@absl//absl/container:flat_hash_set",
    ],
)

cc_library(
    name = "module",
    hdrs = [
        "ir_visitor.h",
        "module.h",
    ],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":data_decl",
        ":operator",
        ":ssa_names",
        ":value",
        "//src/common/utils:types",
        "//src/ir/attributes:attribute",
        "//src/ir/types",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "module_test",
    srcs = ["module_test.cc"],
    deps = [
        ":module",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "ssa_names",
    hdrs = ["ssa_names.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":operator",
        ":value",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "ssa_names_test",
    srcs = ["ssa_names_test.cc"],
    deps = [
        ":module",
        ":operator",
        ":ssa_names",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "storage",
    hdrs = ["storage.h"],
    deps = [
        "//src/ir/types",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "storage_test",
    srcs = ["storage_test.cc"],
    deps = [
        ":storage",
        "//src/common/testing:gtest",
        "//src/ir/types",
    ],
)

cc_library(
    name = "value",
    hdrs = ["value.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":storage",
        "//src/common/utils:overloaded",
        "//src/common/utils:ranges",
        "//src/ir/types",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/status:statusor",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "value_test",
    srcs = ["value_test.cc"],
    deps = [
        ":ir_printer",
        ":module",
        ":ssa_names",
        ":value",
        "//src/common/testing:gtest",
        "//src/ir/types",
        "@absl//absl/hash:hash_testing",
    ],
)

cc_library(
    name = "access_path_selectors",
    hdrs = [
        "access_path_selectors.h",
        "access_path_selectors_set.h",
        "field_selector.h",
        "selector.h",
    ],
    deps = [
        "//src/common/logging",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/hash",
        "@absl//absl/strings",
        "@absl//absl/types:variant",
    ],
)

cc_test(
    name = "access_path_selectors_test",
    srcs = ["access_path_selectors_test.cc"],
    deps = [
        ":access_path_selectors",
        "//src/common/testing:gtest",
        "@absl//absl/hash:hash_testing",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "access_path_selectors_set_test",
    srcs = ["access_path_selectors_set_test.cc"],
    deps = [
        ":access_path_selectors",
        "//src/common/testing:gtest",
        "@absl//absl/container:btree",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "field_selector_test",
    srcs = ["field_selector_test.cc"],
    deps = [
        ":access_path_selectors",
        "//src/common/testing:gtest",
        "@absl//absl/hash:hash_testing",
    ],
)

cc_test(
    name = "selector_test",
    srcs = ["selector_test.cc"],
    deps = [
        ":access_path_selectors",
        "//src/common/testing:gtest",
        "@absl//absl/hash:hash_testing",
    ],
)

cc_library(
    name = "proto_to_ir",
    srcs = ["proto_to_ir.cc"],
    hdrs = ["proto_to_ir.h"],
    deps = [
        ":block_builder",
        ":ir_context",
        ":module",
        ":ssa_names",
        ":value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/proto:ir_cc_proto",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "ir_to_proto",
    srcs = ["ir_to_proto.cc"],
    hdrs = ["ir_to_proto.h"],
    deps = [
        ":ir_context",
        ":module",
        ":ssa_names",
        ":value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/proto:ir_cc_proto",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "ir_to_proto_test",
    srcs = ["ir_to_proto_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":ir_printer",
        ":ir_to_proto",
        ":proto_to_ir",
        "//src/common/proto:proto_message_differencer",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/ir/proto:ir_cc_proto",
        "//src/parser/ir:ir_parser",
    ],
)
