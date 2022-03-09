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
load("//build_defs:arcs.bzl", "arcs_manifest_proto")

package(
    features = ["layering_check"],
    licenses = ["notice"],
)

# TODO(Issue #61): Enable translating these manifests to proto once manifest2proto can parse them.
BUILD_ERROR_ARCS_MANIFEST_PATTERNS = [
    "*fail_field_nullable_inline_entity_direct.arcs",
    "*ok_join_simple_with_nullables.arcs",
]

BUILDING_ARCS_MANIFESTS = glob(
    ["*.arcs"],
    exclude = BUILD_ERROR_ARCS_MANIFEST_PATTERNS,
)

[arcs_manifest_proto(
    name = manifest.replace(".arcs", "_proto"),
    src = manifest,
) for manifest in BUILDING_ARCS_MANIFESTS]
