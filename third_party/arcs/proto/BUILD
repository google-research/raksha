load(
    "//build_defs:native.oss.bzl",
    "cc_proto_library",
    "proto_library",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "proto_files",
    srcs = glob(["*.proto"]),
    visibility = ["//third_party/arcs/tools/manifest2proto:__subpackages__"],
)

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
    protos = [
        ":annotation_proto",
        ":manifest_proto",
        ":policy_proto",
    ],
)
