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
#------------------------------------------------------------------------------

load("//build_defs:test_helpers.bzl", "extracted_datalog_string_test")
load("//build_defs:souffle.bzl", "souffle_cc_library")

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "predicates",
    hdrs = ["predicate.h"],
    deps = [
        "//src/ir:access_path",
        "@absl//absl/strings",
        "@absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "predicate_textproto_to_rule_body_testdata",
    srcs = ["predicate_textproto_to_rule_body_testdata.cc"],
    hdrs = ["predicate_textproto_to_rule_body_testdata.h"],
    deps = [
        "//src/test_utils/dl_string_extractor:datalog_string",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "predicate_test",
    srcs = ["predicate_test.cc"],
    deps = [
        ":predicate_textproto_to_rule_body_testdata",
        ":predicates",
        "//src/common/testing:gtest",
        "//src/ir/proto:predicate",
        "@absl//absl/strings",
    ],
)

extracted_datalog_string_test(
    name = "predicate_extraction_test",
    dl_string_lib = ":predicate_textproto_to_rule_body_testdata",
)
