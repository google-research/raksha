load(
    "//build_defs:souffle.bzl",
    "gen_souffle_cxx_code",
    "souffle_cc_binary",
    "souffle_cc_library",
)

package(default_visibility = ["//src:__subpackages__"])

licenses(["notice"])

cc_test(
    name = "cpp_interface_test",
    srcs = ["cpp_interface_test.cc"],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    linkopts = ["-pthread"],
    deps = [
        ":taint_dl",
        "//src/common/testing:gtest",
        "@souffle//:souffle_include_lib",
    ],
)

gen_souffle_cxx_code(
    name = "taint_cxx",
    src = "taint.dl",
    included_dl_scripts = [
        "authorization_logic.dl",
        "check_predicate.dl",
        "dataflow_graph.dl",
        "operations.dl",
        "tags.dl",
    ],
)

gen_souffle_cxx_code(
    name = "taint_cxx_for_test",
    src = "taint.dl",
    for_test = True,
    included_dl_scripts = [
        "authorization_logic.dl",
        "check_predicate.dl",
        "dataflow_graph.dl",
        "operations.dl",
        "tags.dl",
    ],
)

souffle_cc_library(
    name = "taint_dl",
    src = ":taint_cxx",
)

souffle_cc_binary(
    name = "taint_exec_test",
    testonly = True,
    src = ":taint_cxx_for_test",
)

exports_files([
    "authorization_logic.dl",
    "check_predicate.dl",
    "dataflow_graph.dl",
    "fact_test_helper.dl",
    "operations.dl",
    "tags.dl",
    "taint.dl",
    "may_will.dl",
])
