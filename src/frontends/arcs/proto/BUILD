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
package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "access_path",
    srcs = ["access_path.cc"],
    hdrs = ["access_path.h"],
    deps = [
        "//src/frontends/arcs:access_path",
        "//src/ir:access_path_selectors",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "predicate",
    srcs = ["predicate.cc"],
    hdrs = ["predicate.h"],
    deps = [
        "//src/frontends/arcs",
        "//src/frontends/arcs:access_path",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "tag_check",
    hdrs = ["tag_check.h"],
    deps = [
        ":access_path",
        ":predicate",
        "//src/frontends/arcs",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "tag_claim",
    srcs = ["tag_claim.cc"],
    hdrs = ["tag_claim.h"],
    deps = [
        "//src/frontends/arcs",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "derives_from_claim",
    hdrs = ["derives_from_claim.h"],
    deps = [
        "//src/frontends/arcs",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "handle_connection_spec",
    srcs = ["handle_connection_spec.cc"],
    hdrs = ["handle_connection_spec.h"],
    deps = [
        "//src/frontends/arcs",
        "//src/ir/types",
        "//src/ir/types/proto",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "particle_spec",
    srcs = ["particle_spec.cc"],
    hdrs = ["particle_spec.h"],
    deps = [
        ":derives_from_claim",
        ":handle_connection_spec",
        ":predicate",
        ":tag_check",
        ":tag_claim",
        "//src/frontends/arcs",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_library(
    name = "system_spec",
    srcs = ["system_spec.cc"],
    hdrs = ["system_spec.h"],
    deps = [
        ":particle_spec",
        "//src/frontends/arcs",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "access_path_test",
    srcs = ["access_path_test.cc"],
    deps = [
        ":access_path",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "access_path_root_test",
    srcs = ["access_path_root_test.cc"],
    deps = [
        ":access_path",
        "//src/common/proto:proto_message_differencer",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "access_path_selector_test",
    srcs = ["access_path_selector_test.cc"],
    deps = [
        ":access_path",
        "//src/common/proto:proto_message_differencer",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
    ],
)
