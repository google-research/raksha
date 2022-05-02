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
    name = "proto",
    srcs = [
        "entity_type.cc",
        "primitive_type.cc",
        "schema.cc",
        "type.cc",
    ],
    hdrs = [
        "entity_type.h",
        "primitive_type.h",
        "schema.h",
        "type.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "entity_type_test",
    srcs = ["entity_type_test.cc"],
    deps = [
        ":proto",
        "//src/common/proto:proto_message_differencer",
        "//src/common/proto:protobuf",
        "//src/common/testing:gtest",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "primitive_type_test",
    srcs = ["primitive_type_test.cc"],
    deps = [
        ":proto",
        "//src/common/testing:gtest",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)
