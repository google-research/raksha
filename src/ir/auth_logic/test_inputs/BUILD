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
package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

[sh_test(
    name = "test_%s" % test_name,
    srcs = ["test_authlogic.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:raksha_policy_datalog_emitter)",
        "$(location :%s.auth)" % test_name,
        "$(location :%s.dl)" % test_name,
        "1",
        "$(location :%s.queries)" % test_name,
    ],
    data = [
        ":%s.auth" % test_name,
        ":%s.dl" % test_name,
        ":%s.queries" % test_name,
        "//src/backends/policy_engine/souffle:raksha_policy_datalog_emitter",
        "//third_party/mcpp",
        "//third_party/souffle:main",
    ],
    env = {
        "SOUFFLE_BIN": "$(rootpath //third_party/souffle:main)",
    },
) for test_name in [
    "canActAs",
    "conditions",
    "negation",
    "multiverse_handling",
    "delegation",
    "comparisons_with_delegation",
    "basic_num_comparisons",
]]

[sh_test(
    name = "test_%s" % test_name,
    srcs = ["test_authlogic.sh"],
    args = [
        "$(location //src/backends/policy_engine/souffle:raksha_policy_datalog_emitter)",
        "$(location :%s.auth)" % test_name,
        "$(location :%s.dl)" % test_name,
    ],
    data = [
        ":%s.auth" % test_name,
        ":%s.dl" % test_name,
        "//src/backends/policy_engine/souffle:raksha_policy_datalog_emitter",
        "//third_party/souffle:main",
    ],
    env = {
        "SOUFFLE_BIN": "$(rootpath //third_party/souffle:main)",
    },
) for test_name in [
    "type_declarations",
    "test_decl_skip",
    "num_strings_in_names",
    "hyphenIDs",
    "dot_in_names",
]]
