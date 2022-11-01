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

load("//build_defs:test_helpers.bzl", "extracted_datalog_string_test")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "arcs",
    srcs = [
        "access_path_root.cc",
        "particle_spec.cc",
    ],
    hdrs = [
        "datalog_print_context.h",
        "derives_from_claim.h",
        "edge.h",
        "handle_connection_spec.h",
        "particle_spec.h",
        "predicate.h",
        "system_spec.h",
        "tag_check.h",
        "tag_claim.h",
    ],
    deps = [
        ":access_path",
        "//src/common/containers:hash_map",
        "//src/common/logging",
        "//src/frontends/arcs/proto:access_path",
        "//src/ir/types",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "access_path",
    hdrs = [
        "access_path.h",
        "access_path_root.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/ir:access_path_selectors",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:variant",
    ],
)

cc_test(
    name = "access_path_test",
    srcs = ["access_path_test.cc"],
    deps = [
        ":access_path",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/hash:hash_testing",
    ],
)

cc_test(
    name = "edge_test",
    srcs = ["edge_test.cc"],
    deps = [
        ":access_path",
        ":arcs",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "system_spec_test",
    srcs = ["system_spec_test.cc"],
    deps = [
        ":arcs",
        "//src/common/logging",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "tag_claim_test",
    srcs = ["tag_claim_test.cc"],
    deps = [
        ":access_path",
        ":arcs",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:tag_claim",
        "//src/ir:access_path_selectors",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "tag_check_test",
    srcs = ["tag_check_test.cc"],
    deps = [
        ":arcs",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:predicate",
        "//src/frontends/arcs/proto:tag_check",
        "//src/ir:access_path_selectors",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "access_path_root_test",
    srcs = ["access_path_root_test.cc"],
    deps = [
        ":access_path",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/hash:hash_testing",
    ],
)

cc_test(
    name = "derives_from_claim_test",
    srcs = ["derives_from_claim_test.cc"],
    deps = [
        ":arcs",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:access_path",
        "//src/frontends/arcs/proto:derives_from_claim",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "predicate_textproto_to_rule_body_testdata",
    testonly = True,
    srcs = ["predicate_textproto_to_rule_body_testdata.cc"],
    hdrs = ["predicate_textproto_to_rule_body_testdata.h"],
    deps = [
        "//src/common/utils/test/dl_string_extractor:datalog_string",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "predicate_test",
    srcs = ["predicate_test.cc"],
    deps = [
        ":arcs",
        ":predicate_textproto_to_rule_body_testdata",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:predicate",
        "//third_party/arcs/proto:manifest_cc_proto",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "handle_connection_spec_test",
    srcs = ["handle_connection_spec_test.cc"],
    deps = [
        ":arcs",
        "//src/common/proto:proto_message_differencer",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:handle_connection_spec",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "particle_spec_test",
    srcs = ["particle_spec_test.cc"],
    deps = [
        ":arcs",
        "//src/common/logging",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/frontends/arcs/proto:particle_spec",
        "//src/ir/types",
    ],
)

extracted_datalog_string_test(
    name = "predicate_extraction_test",
    dl_string_lib = ":predicate_textproto_to_rule_body_testdata",
)
