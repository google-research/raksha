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
#-------------------------------------------------------------------------------

licenses(["notice"])

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "ast",
    hdrs = [
        "ast.h",
    ],
    visibility = ["//test:__pkg__"],
    deps = [
        "//src/common/logging",
        "//src/utils:overloaded",
        "@absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "lowering_ast_datalog",
    srcs = ["lowering_ast_datalog.cc"],
    hdrs = [
        "datalog_ir.h",
        "lowering_ast_datalog.h",
        "map_iter.h",
        "move_append.h",
        "souffle_emitter.h"    # TODO move this
    ],
    visibility = ["//test:__pkg__"],
    deps = [
        "//src/common/logging",
        "//src/ir/auth_logic:ast",
        "//src/utils:overloaded",
        "@absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "souffle_emitter",
    srcs = ["souffle_emitter_build_stub.cc"],
    hdrs = [
        "souffle_emitter.h",
    ],
    deps = [
        "//src/ir/auth_logic:ast",
        "//src/ir/auth_logic:lowering_ast_datalog",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "ast_test",
    srcs = ["ast_test.cc"],
    deps = [
        ":ast",
        "//src/common/testing:gtest",
    ],
)
