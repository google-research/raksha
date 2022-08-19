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
load(
    "//build_defs:package_groups.oss.bzl",
    "frontend_packages",
)

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "policy",
    hdrs = ["policy.h"],
    visibility = [
        "//:__pkg__",
        "//src:__subpackages__",
    ] + frontend_packages,
    deps = ["@absl//absl/strings"],
)

cc_library(
    name = "sql_policy_rule_policy",
    hdrs = ["sql_policy_rule_policy.h"],
    visibility = [
        "//:__pkg__",
        "//src:__subpackages__",
    ] + frontend_packages,
    deps = [
        ":policy",
        "@absl//absl/status",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "auth_logic_policy",
    hdrs = ["auth_logic_policy.h"],
    visibility = [
        "//:__pkg__",
        "//src:__subpackages__",
    ] + frontend_packages,
    deps = [
        ":policy",
        "@absl//absl/status",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "policy_checker",
    hdrs = ["policy_checker.h"],
    deps = [
        ":policy",
        "//src/ir:module",
    ],
)

cc_library(
    name = "dp_parameter_policy",
    hdrs = ["dp_parameter_policy.h"],
    deps = [
        ":policy",
        "@absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "dp_parameter_policy_test",
    srcs = ["dp_parameter_policy_test.cc"],
    deps = [
        ":dp_parameter_policy",
        "//src/common/testing:gtest",
        "@absl//absl/strings",
    ],
)
