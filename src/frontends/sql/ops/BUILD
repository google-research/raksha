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
    default_visibility = [
        # The policy checker test currently requires specific operations from
        # the SQL frontend because that's where tainting via operations is most
        # developed. We should refactor to remove this test in the future.
        "//src/backends/policy_engine/souffle:__pkg__",
        "//src/frontends/sql:__subpackages__",
    ],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "example_value_test_helper",
    testonly = True,
    hdrs = ["example_value_test_helper.h"],
    deps = [
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "@absl//absl/strings",
    ],
)

cc_library(
    name = "literal_op",
    srcs = ["literal_op.cc"],
    hdrs = ["literal_op.h"],
    deps = [
        ":sql_op",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:string_attribute",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "literal_op_test",
    srcs = ["literal_op_test.cc"],
    deps = [
        ":literal_op",
        ":sql_op",
        "//src/common/testing:gtest",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:string_attribute",
    ],
)

cc_library(
    name = "merge_op",
    srcs = ["merge_op.cc"],
    hdrs = ["merge_op.h"],
    deps = [
        ":sql_op",
        "//src/common/utils:ranges",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "@absl//absl/algorithm:container",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "merge_op_test",
    srcs = ["merge_op_test.cc"],
    deps = [
        ":example_value_test_helper",
        ":merge_op",
        ":sql_op",
        "//src/common/testing:gtest",
        "//src/common/utils:ranges",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
    ],
)

cc_library(
    name = "sql_op",
    hdrs = ["sql_op.h"],
    deps = [
        ":op_traits",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:operator",
    ],
)

cc_test(
    name = "sql_op_test",
    srcs = ["sql_op_test.cc"],
    deps = [
        ":op_traits",
        ":sql_op",
        "//src/common/testing:gtest",
        "//src/ir:ir_context",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
    ],
)

cc_library(
    name = "tag_transform_op",
    srcs = ["tag_transform_op.cc"],
    hdrs = ["tag_transform_op.h"],
    deps = [
        ":op_traits",
        ":sql_op",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
    ],
)

cc_test(
    name = "tag_transform_op_test",
    srcs = ["tag_transform_op_test.cc"],
    deps = [
        ":example_value_test_helper",
        ":op_traits",
        ":sql_op",
        ":tag_transform_op",
        "//src/common/testing:gtest",
        "//src/ir:ir_context",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
    ],
)

cc_library(
    name = "sql_output_op",
    hdrs = ["sql_output_op.h"],
    deps = [
        ":sql_op",
        "//src/ir:ir_context",
        "//src/ir:module",
        "//src/ir:value",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "sql_output_op_test",
    srcs = ["sql_output_op_test.cc"],
    deps = [
        ":example_value_test_helper",
        ":sql_op",
        ":sql_output_op",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/common/utils:ranges",
        "//src/ir:value",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
    ],
)

cc_library(
    name = "op_traits",
    hdrs = ["op_traits.h"],
    deps = [
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "op_traits_test",
    srcs = ["op_traits_test.cc"],
    deps = [
        ":op_traits",
        "//src/common/testing:gtest",
    ],
)
