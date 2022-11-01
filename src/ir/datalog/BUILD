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
    name = "program",
    hdrs = ["program.h"],
    deps = [
        "//src/common/containers:hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "predicate_test",
    srcs = ["predicate_test.cc"],
    deps = [
        ":program",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "argument_type_factory_test",
    srcs = ["argument_type_factory_test.cc"],
    deps = [
        ":program",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "is_numeric_operator_test",
    srcs = ["is_numeric_operator_test.cc"],
    deps = [
        ":program",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "hash_test",
    srcs = ["hash_test.cc"],
    deps = [
        ":program",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/hash:hash_testing",
    ],
)

# This library contains our C++ model of the value types of Souffle.
cc_library(
    name = "value",
    hdrs = [
        "value.h",
    ],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "input_relation_fact",
    hdrs = ["input_relation_fact.h"],
    deps = [
        ":value",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "attribute",
    hdrs = ["attribute.h"],
    deps = [":value"],
)

cc_test(
    name = "attribute_test",
    srcs = ["attribute_test.cc"],
    deps = [
        ":attribute",
        "//src/common/testing:gtest",
    ],
)

# The interface to the Raksha analysis, as described by the C++ version of the
# Datalog relations that are input/output relations.
cc_library(
    name = "raksha_relation_interface",
    hdrs = [
        "operation.h",
    ],
    deps = [
        ":attribute",
        ":input_relation_fact",
        ":value",
    ],
)

cc_test(
    name = "value_test",
    srcs = ["value_test.cc"],
    deps = [
        ":value",
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
        "@fuzztest//fuzztest",
    ],
)

cc_test(
    name = "input_relation_fact_test",
    srcs = ["input_relation_fact_test.cc"],
    deps = [
        ":input_relation_fact",
        ":value",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "operation_test",
    srcs = ["operation_test.cc"],
    deps = [
        ":input_relation_fact",
        ":raksha_relation_interface",
        ":value",
        "//src/common/testing:gtest",
    ],
)
