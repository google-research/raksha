cc_library(
    name = "ir_proto_to_datalog_lib",
    srcs = glob(["*.cc"]),
    deps = [
        "//third_party/arcs/proto:manifest_cc_proto"
    ],
    linkopts = select({
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-ldl",
        ],
        "//conditions:default": [
        ],
    }),
)
