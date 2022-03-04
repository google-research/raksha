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
    "cc_proto_library",
    "proto_library",
)

licenses(["notice"])

proto_library(
    name = "sql_ir_proto",
    srcs = ["sql_ir.proto"],
)

cc_proto_library(
    name = "sql_ir_cc_proto",
    protos = [
        ":sql_ir_proto",
    ],
)

cc_library(
    name = "decoder_context",
    srcs = ["decoder_context.cc"],
    hdrs = ["decoder_context.h"],
    deps = [
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:value",
        "@absl//absl/container:flat_hash_map",
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
    deps = [
        ":decoder_context",
        ":sql_ir_cc_proto",
        "//src/common/logging",
        "//src/ir",
        "//src/ir:attribute",
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:ssa_names",
        "//src/ir:value",
    ],
)

cc_test(
    name = "decoder_context_test",
    srcs = ["decoder_context_test.cc"],
    copts = ["-fexceptions"],
    deps = [
        ":decoder_context",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "decode_test",
    srcs = ["decode_test.cc"],
    deps = [
        ":decode",
        ":decoder_context",
        "//src/common/testing:gtest",
        "//src/frontends/sql:sql_ir_cc_proto",
        "@absl//absl/strings",
    ],
)
