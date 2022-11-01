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
    name = "dot_generator",
    srcs = [
        "dot_generator.cc",
    ],
    hdrs = [
        "dot_generator.h",
        "dot_generator_config.h",
    ],
    deps = [
        "//src/common/containers:hash_map",
        "//src/common/containers:hash_set",
        "//src/ir:ir_printer",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "@com_google_absl//absl/container:btree",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "dot_generator_test",
    srcs = ["dot_generator_test.cc"],
    deps = [
        ":dot_generator",
        "//src/common/testing:gtest",
        "//src/ir:block_builder",
        "//src/ir:ir_context",
        "//src/ir:ir_printer",
        "//src/ir:module",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:float_attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "@com_google_absl//absl/strings",
    ],
)
