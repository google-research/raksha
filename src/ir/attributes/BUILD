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
load("//build_defs:package_groups.oss.bzl", "frontend_packages")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "attribute",
    hdrs = ["attribute.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        "//src/common/logging",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "attribute_test",
    srcs = ["attribute_test.cc"],
    deps = [
        ":attribute",
        ":float_attribute",
        ":int_attribute",
        ":string_attribute",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "float_attribute",
    hdrs = ["float_attribute.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":attribute",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "float_attribute_test",
    srcs = ["float_attribute_test.cc"],
    deps = [
        ":float_attribute",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "int_attribute",
    hdrs = ["int_attribute.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":attribute",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "int_attribute_test",
    srcs = ["int_attribute_test.cc"],
    deps = [
        ":int_attribute",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "string_attribute",
    hdrs = ["string_attribute.h"],
    visibility = ["//src:__subpackages__"] + frontend_packages,
    deps = [
        ":attribute",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "string_attribute_test",
    srcs = ["string_attribute_test.cc"],
    deps = [
        ":string_attribute",
        "//src/common/testing:gtest",
    ],
)
