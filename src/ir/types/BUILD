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

licenses(["notice"])

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "types",
    srcs = [
        "schema.cc",
        "type_factory.cc",
    ],
    hdrs = [
        "entity_type.h",
        "primitive_type.h",
        "schema.h",
        "type.h",
        "type_factory.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/ir:access_path",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/hash",
    ],
)

cc_test(
    name = "primitive_type_test",
    srcs = ["primitive_type_test.cc"],
    deps = [
        ":types",
        "//src/common/testing:gtest",
        "//src/ir:access_path",
    ],
)

cc_test(
    name = "type_test",
    srcs = ["type_test.cc"],
    deps = [
        ":types",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/ir:access_path",
        "//src/ir/proto:types",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings",
    ],
)
