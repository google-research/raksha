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
load("//build_defs:arcs.bzl", "arcs_manifest_proto")

package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

filegroup(
    name = "auth_logic",
    testonly = True,
    srcs = [
        "empty_auth_logic.auth",
        "empty_auth_logic.dl",
        "says_owns_tag.auth",
        "says_owns_tag.dl",
        "simple_auth_logic.auth",
        "simple_auth_logic.dl",
    ],
)

arcs_manifest_proto(
    name = "ok_claim_propagates_proto",
    src = "ok_claim_propagates.arcs",
    deps = [],
)

filegroup(
    name = "ok_claim_propagates",
    srcs = [
        "ok_claim_propagates.arcs",
        "ok_claim_propagates.auth",
        "ok_claim_propagates.dl",
        ":ok_claim_propagates_proto",
    ],
)
