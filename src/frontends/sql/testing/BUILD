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
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "merge_operation_view",
    testonly = True,
    hdrs = ["merge_operation_view.h"],
    visibility = ["//src/frontends/sql:__subpackages__"],
    deps = [
        ":operation_view_utils",
        "//src/common/logging",
        "//src/frontends/sql:decoder_context",
        "//src/ir:module",
        "//src/ir:operator",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "utils",
    testonly = True,
    hdrs = ["utils.h"],
    visibility = ["//src/frontends/sql:__subpackages__"],
    deps = [
        "//src/common/logging",
        "//src/frontends/sql:decoder_context",
        "//src/ir:module",
        "//src/ir:value",
    ],
)

cc_library(
    name = "literal_operation_view",
    testonly = True,
    hdrs = ["literal_operation_view.h"],
    visibility = ["//src/frontends/sql:__subpackages__"],
    deps = [
        ":operation_view_utils",
        "//src/common/logging",
        "//src/frontends/sql:decoder_context",
        "//src/ir:module",
        "//src/ir:operator",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "tag_transform_operation_view",
    testonly = True,
    hdrs = ["tag_transform_operation_view.h"],
    visibility = ["//src/frontends/sql:__subpackages__"],
    deps = [
        ":operation_view_utils",
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "//src/frontends/sql:decoder_context",
        "//src/frontends/sql/testing:literal_operation_view",
        "//src/frontends/sql/testing:merge_operation_view",
        "//src/frontends/sql/testing:utils",
        "//src/ir:module",
        "//src/ir:operator",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "operation_view_utils",
    testonly = True,
    srcs = ["operation_view_utils.cc"],
    hdrs = ["operation_view_utils.h"],
    deps = [
        "//src/ir:value",
        "@absl//absl/strings",
    ],
)
