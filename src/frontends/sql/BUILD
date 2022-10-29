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

load(
    "//build_defs:native.oss.bzl",
    "proto_library",
    "raksha_cc_proto_library",
)

package(
    features = ["layering_check"],
    licenses = ["notice"],
)

# Expose the proto library to the top-level package. While this is extraneous
# in the public version of Raksha, it allows us to "trampoline" exporting
# these protos to tools in the Google-internal repo.
proto_library(
    name = "sql_ir_proto",
    srcs = ["sql_ir.proto"],
    visibility = ["//:__pkg__"],
)

raksha_cc_proto_library(
    name = "sql_ir_cc_proto",
    visibility = ["//:__pkg__"],
    deps = [
        ":sql_ir_proto",
    ],
)

cc_library(
    name = "decoder_context",
    srcs = ["decoder_context.cc"],
    hdrs = ["decoder_context.h"],
    visibility = [
        ":__subpackages__",
        "//:__pkg__",
        # Make this visible to the datalog lowering visitor so it can use the default output value constant.
        "//src/backends/policy_engine/souffle:__pkg__",
    ],
    deps = [
        "//src/common/utils:map_iter",
        "//src/frontends/sql/ops:literal_op",
        "//src/frontends/sql/ops:merge_op",
        "//src/frontends/sql/ops:sql_output_op",
        "//src/frontends/sql/ops:tag_transform_op",
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_library(
    name = "decode",
    srcs = [
        "decode.cc",
    ],
    hdrs = [
        "decode.h",
    ],
    visibility = [
        "//:__pkg__",
    ],
    deps = [
        ":decoder_context",
        ":sql_ir_cc_proto",
        "//src/frontends/sql/ops:sql_output_op",
        "//src/ir:value",
    ],
)

cc_library(
    name = "driver",
    srcs = ["driver.cc"],
    hdrs = ["driver.h"],
    visibility = [
        "//:__pkg__",
    ],
    deps = [
        ":decode",
        ":sql_ir_cc_proto",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine/chaotic:chaotic_policy_checker",
        "//src/backends/policy_engine/chaotic:sql_policy_rules",
        #"//third_party/raksha/src/backends/policy_engine/souffle:souffle_policy_checker_sql",
    ],
)

cc_test(
    name = "driver_test",
    srcs = ["driver_test.cc"],
    deps = [
        ":driver",
        ":sql_ir_cc_proto",
        "//src/backends/policy_engine:policy",
        "//src/backends/policy_engine:sql_policy_rule_policy",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/ir:ir_printer",
    ],
)

cc_test(
    name = "decoder_context_test",
    srcs = ["decoder_context_test.cc"],
    deps = [
        ":decoder_context",
        "//src/common/testing:gtest",
        "//src/common/utils:map_iter",
        "//src/frontends/sql/ops:merge_op",
        "//src/frontends/sql/ops:tag_transform_op",
    ],
)

cc_library(
    name = "name_and_string_test",
    testonly = True,
    hdrs = ["name_and_string_test.h"],
    deps = [
        ":decode",
        ":decoder_context",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/sql:sql_ir_cc_proto",
        "//src/frontends/sql/testing:utils",
        "//src/ir:ir_context",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "decode_expr_death_test",
    srcs = ["decode_expr_death_test.cc"],
    deps = [
        ":decode",
        ":decoder_context",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/sql:sql_ir_cc_proto",
        "//src/frontends/sql/testing:utils",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "decode_literal_test",
    srcs = ["decode_literal_test.cc"],
    deps = [
        ":decoder_context",
        ":name_and_string_test",
        "//src/common/testing:gtest",
        "//src/frontends/sql/ops:literal_op",
        "//src/frontends/sql/testing:utils",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "decode_source_column_test",
    srcs = ["decode_source_column_test.cc"],
    deps = [
        ":decoder_context",
        ":name_and_string_test",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "decode_merge_operation_test",
    srcs = ["decode_merge_operation_test.cc"],
    deps = [
        ":decode",
        ":decoder_context",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/sql:sql_ir_cc_proto",
        "//src/frontends/sql/ops:merge_op",
        "//src/frontends/sql/testing:utils",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "decode_tag_transform_operation_test",
    srcs = ["decode_tag_transform_operation_test.cc"],
    deps = [
        ":decode",
        ":decoder_context",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/common/utils:map_iter",
        "//src/frontends/sql:sql_ir_cc_proto",
        "//src/frontends/sql/ops:tag_transform_op",
        "//src/frontends/sql/testing:utils",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
