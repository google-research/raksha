#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#----------------------------------------------------------------------------

cc_library(
    name = "datalog_lowering_visitor",
    hdrs = ["datalog_lowering_visitor.h"],
    deps = [
        ":fact",
        ":raksha",
        ":value",
        "//src/common/logging",
        "//src/ir:module",
        "//src/ir:operator",
    ],
)

cc_test(
    name = "datalog_lowering_visitor_test",
    srcs = ["datalog_lowering_visitor_test.cc"],
    deps = [
        ":datalog_lowering_visitor",
        ":raksha",
        ":value",
        "//src/common/testing:gtest",
        "@absl//absl/strings",
    ],
)
