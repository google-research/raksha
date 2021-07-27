load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

package(default_visibility = ["//src:__subpackages__"])

licenses(["notice"])

cc_test(
    name = "cpp_interface_test",
    srcs = ["cpp_interface_test.cc"],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    linkopts = ["-pthread"],
    deps = [
        ":taint_dl",
        "//src/common/testing:gtest",
        "@souffle//:souffle_lib",
    ],
)

souffle_cc_library(
    name = "taint_dl",
    src = "taint.dl",
)

exports_files([
    "fact_test_helper.dl",
    "flat_graph_ir.dl",
    "auth_logic_interface.dl",
    "edges_from_graph.dl",
])
