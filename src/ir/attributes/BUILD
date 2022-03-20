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
    name = "attribute",
    hdrs = ["attribute.h"],
    deps = [
        "//src/common/utils:intrusive_ptr",
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
        ":int_attribute",
        ":string_attribute",
        "//src/common/testing:gtest",
        "//src/common/utils:intrusive_ptr",
    ],
)

cc_library(
    name = "int_attribute",
    hdrs = ["int_attribute.h"],
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
