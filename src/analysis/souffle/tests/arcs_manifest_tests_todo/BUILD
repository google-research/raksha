load("//build_defs:arcs.bzl", "arcs_manifest_proto")

# TODO(Issue #61): Enable translating these manifests to proto once manifest2proto can parse them.
BUILD_ERROR_ARCS_MANIFEST_PATTERNS = [
    "*fail_field_nullable_inline_entity_direct.arcs",
    "*ok_join_simple_with_nullables.arcs",
]

BUILDING_ARCS_MANIFESTS = glob(
    ["*.arcs"],
    exclude = BUILD_ERROR_ARCS_MANIFEST_PATTERNS
)

[arcs_manifest_proto(
    name = manifest.replace(".arcs", "_proto"),
    src = manifest,
 ) for manifest in BUILDING_ARCS_MANIFESTS]

