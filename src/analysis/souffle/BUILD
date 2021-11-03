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
    "access_path.dl",
    "authorization_logic.dl",
    "dataflow_graph.dl",
    "fact_test_helper.dl",
    "flat_graph_ir.dl",
    "tags.dl",
    "may_will.dl",
])
