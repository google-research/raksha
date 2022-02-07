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
    name = "intrusive_ptr",
    hdrs = ["intrusive_ptr.h"],
    deps = [
        ":ref_counted",
    ],
)

cc_test(
    name = "intrusive_ptr_with_ref_counted_test",
    srcs = ["intrusive_ptr_with_ref_counted_test.cc"],
    deps = [
        ":intrusive_ptr",
        ":ref_counted",
        "//src/common/testing:gtest",
    ],
)

cc_test(
    name = "intrusive_ptr_test",
    srcs = ["intrusive_ptr_test.cc"],
    deps = [
        ":intrusive_ptr",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "ref_counted",
    hdrs = ["ref_counted.h"],
    deps = [
        ":ref_counter",
    ],
)

cc_test(
    name = "ref_counted_test",
    srcs = ["ref_counted_test.cc"],
    deps = [
        ":ref_counted",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "ref_counter",
    hdrs = ["ref_counter.h"],
    deps = ["//src/common/logging"],
)

cc_test(
    name = "ref_counter_test",
    srcs = ["ref_counter_test.cc"],
    deps = [
        ":ref_counter",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "overloaded",
    hdrs = ["overloaded.h"],
    deps = [],
)

cc_test(
    name = "overloaded_test",
    srcs = ["overloaded_test.cc"],
    deps = [
        ":overloaded",
        "//src/common/testing:gtest",
    ],
)

cc_library(
    name = "ranges",
    hdrs = ["ranges.h"],
    deps = [],
)

cc_test(
    name = "ranges_test",
    srcs = ["ranges_test.cc"],
    deps = [
        ":ranges",
        "//src/common/testing:gtest",
    ],
)
