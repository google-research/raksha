load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

cc_binary(
    name = "cpp_interface",
    srcs = ["cpp_interface.cc"],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    deps = [
        "@souffle//:souffle_lib",
        ":taint_dl"
    ],
)

souffle_cc_library(
    name = "taint_dl",
    src = "taint.dl",
)
