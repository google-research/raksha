load("//build_defs:arcs.bzl", "arcs_manifest_proto")

BUILDING_ARCS_MANIFESTS = glob(["*.arcs"])

[arcs_manifest_proto(
    name = manifest.replace(".arcs", "_proto"),
    src = manifest,
) for manifest in BUILDING_ARCS_MANIFESTS]
