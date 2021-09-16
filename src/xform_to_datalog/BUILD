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

cc_library(
    name = "datalog_facts",
    hdrs = ["datalog_facts.h"],
    deps = [
        ":manifest_datalog_facts",
        "//src/ir",
        "//src/xform_to_datalog/arcs_manifest_tree",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "datalog_facts_test",
    srcs = ["datalog_facts_test.cc"],
    deps = [
        ":datalog_facts",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "manifest_datalog_facts",
    srcs = ["manifest_datalog_facts.cc"],
    hdrs = ["manifest_datalog_facts.h"],
    deps = [
        "//src/ir",
        "//src/ir/proto:types",
        "//src/xform_to_datalog/arcs_manifest_tree",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "manifest_datalog_facts_test",
    srcs = ["manifest_datalog_facts_test.cc"],
    deps = [
        ":manifest_datalog_facts",
        "//src/common/testing:gtest",
    ],
)

cc_binary(
    name = "generate_datalog_program",
    srcs = ["generate_datalog_program.cc"],
    deps = [
        ":datalog_facts",
        "//src/common/logging",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
    ],
)
