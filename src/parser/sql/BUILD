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
        "//intelligence/federated_perception:__subpackages__",
        "//src:__subpackages__",
    ],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_library(
    name = "sql_parser",
    srcs = ["sql_parser.cc"],
    hdrs = ["sql_parser.h"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        "//src/common/containers:hash_map",
        "//src/common/logging",
        "//src/common/utils:map_iter",
        "@com_google_zetasql//zetasql/public:analyzer",
        "@com_google_zetasql//zetasql/public:parse_helpers",
    ],
)

cc_test(
    name = "sql_parser_test",
    srcs = ["sql_parser_test.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        ":sql_parser",
        "//src/common/testing:gtest",
    ],
)
