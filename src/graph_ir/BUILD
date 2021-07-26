load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

cc_binary(
    name = "low_graph_ir",
    srcs = ["low_graph_ir.cc", "low_graph_ir.h"],
    copts = [
        "-std=c++17",
    ],
)

cc_binary(
    name = "low_graph_souffle_emitter",
    srcs = [
        "low_graph_souffle_emitter.cc",
        "low_graph_souffle_emitter.h"
    ],
    deps = ["low_graph_ir"],
    copts = [
        "-std=c++17",
    ],
)
