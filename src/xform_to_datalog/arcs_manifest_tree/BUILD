#-------------------------------------------------------------------------------
# Copyright 2021 Google LLC
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
#-------------------------------------------------------------------------------

package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "arcs_manifest_tree",
    srcs = [
        "particle_spec.cc",
    ],
    hdrs = [
        "particle_spec.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/ir",
        "//src/ir/proto:derives_from_claim",
        "//src/ir/proto:handle_connection_spec",
        "//src/ir/proto:predicate",
        "//src/ir/proto:tag_check",
        "//src/ir/proto:tag_claim",
        "//src/ir/proto:types",
        "//src/ir/types",
        "//third_party/arcs/proto:manifest_cc_proto",
        "@absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "particle_spec_test",
    srcs = ["particle_spec_test.cc"],
    deps = [
        ":arcs_manifest_tree",
        "//src/common/testing:gtest",
    ],
)
