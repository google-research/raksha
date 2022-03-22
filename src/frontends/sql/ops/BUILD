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
