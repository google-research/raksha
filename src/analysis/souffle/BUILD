load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

package(default_visibility=["//visibility:public"])

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

cc_binary(
    name = "find_failing_tag_checks",
    srcs = ["find_failing_tag_checks.cc"],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    deps = [
        "@souffle//:souffle_lib",
        ":find_failing_tag_checks_dl"
    ],
)

souffle_cc_library(
    name = "find_failing_tag_checks_dl",
    src = "find_failing_tag_checks.dl",
)
