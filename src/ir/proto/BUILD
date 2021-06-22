load(
    "//build_defs:native.oss.bzl",
    "cc_proto_library",
    "proto_library",
)

licenses(["notice"])

proto_library(
    name = "annotation_proto",
    srcs = ["annotation.proto"],
)

proto_library(
    name = "manifest_proto",
    srcs = ["manifest.proto"],
    deps = [
        ":annotation_proto",
        ":policy_proto",
    ],
)

proto_library(
    name = "policy_proto",
    srcs = ["policy.proto"],
    deps = [":annotation_proto"],
)

cc_proto_library(
    name = "manifest_cc_proto",
    deps = [":manifest_proto"],
)
