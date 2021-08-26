load("//build_defs:arcs.bzl", "arcs_manifest_proto")

arcs_manifest_proto(
    name = "consume",
    src = "Consume.arcs",
    deps = [
        "ConsumeProduce.arcs",
        "Produce.arcs",
        "Schemas.arcs",
    ],
)

arcs_manifest_proto(
    name = "vision_pipeline",
    src = "VisionPipeline.arcs",
    deps = [],
)
