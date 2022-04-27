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
    srcs = ["datalog_lowering_visitor.cc"],
    hdrs = [
        "datalog_lowering_visitor.h",
        "raksha_datalog_facts.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/frontends/sql:decoder_context",
        "//src/ir:ir_traversing_visitor",
        "//src/ir:module",
        "//src/ir:operator",
        "//src/ir/attributes:attribute",
        "//src/ir/attributes:int_attribute",
        "//src/ir/attributes:string_attribute",
        "//src/ir/datalog:input_relation_fact",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
    ],
)

cc_test(
    name = "datalog_lowering_visitor_test",
    srcs = ["datalog_lowering_visitor_test.cc"],
    deps = [
        ":datalog_lowering_visitor",
        "//src/common/testing:gtest",
        "//src/ir:block_builder",
        "//src/ir:module",
        "//src/ir/datalog:raksha_relation_interface",
        "//src/ir/datalog:value",
        "@absl//absl/strings",
    ],
)
