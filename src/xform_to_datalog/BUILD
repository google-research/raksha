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
package(default_visibility = ["//src:__subpackages__"])

licenses(["notice"])

cc_library(
    name = "authorization_logic",
    hdrs = ["authorization_logic.h"],
    deps = ["//rust/tools/authorization-logic:authorization_logic_lib"],
)

cc_test(
    name = "authorization_logic_test",
    srcs = ["authorization_logic_test.cc"],
    data = ["//src/xform_to_datalog/testdata:auth_logic"],
    deps = [
        ":authorization_logic",
        "//src/test_utils:utils",
        "//src/common/logging",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "authorization_logic_datalog_facts",
    srcs = ["authorization_logic_datalog_facts.cc"],
    hdrs = ["authorization_logic_datalog_facts.h"],
    linkstatic = True,
    deps = [
        ":authorization_logic",
        "//src/common/logging",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "authorization_logic_datalog_facts_test",
    srcs = ["authorization_logic_datalog_facts_test.cc"],
    data = ["//src/xform_to_datalog/testdata:auth_logic"],
    deps = [
        ":authorization_logic_datalog_facts",
        "//src/test_utils:utils",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "datalog_facts",
    hdrs = ["datalog_facts.h"],
    deps = [
        ":authorization_logic_datalog_facts",
        ":manifest_datalog_facts",
        "//src/ir",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "datalog_facts_test",
    srcs = ["datalog_facts_test.cc"],
    data = ["//src/xform_to_datalog/testdata:auth_logic"],
    deps = [
        "//src/test_utils:utils",
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
        "//src/ir/proto:particle_spec",
        "//src/ir/proto:system_spec",
        "//src/ir/proto:types",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "manifest_datalog_facts_test",
    srcs = ["manifest_datalog_facts_test.cc"],
    deps = [
        ":manifest_datalog_facts",
        "//src/common/testing:gtest",
        "//src/utils:ranges",
    ],
)

cc_binary(
    name = "generate_datalog_program",
    srcs = ["generate_datalog_program.cc"],
    deps = [
        ":datalog_facts",
        "//src/ir/proto:system_spec",
        "//src/common/logging",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
    ],
)

sh_test(
    name = "generate_datalog_program_test",
    srcs = ["generate_datalog_program_test.sh"],
    data = [
        ":generate_datalog_program",
        "//src/xform_to_datalog/testdata:ok_claim_propagates",
    ],
)
