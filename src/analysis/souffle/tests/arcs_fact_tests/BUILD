load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

FAILURE_DL_TEST_FILES = glob(["*_expect_fails.dl"])

ALL_DL_TEST_FILES = glob(["*.dl"])

[souffle_cc_library(
    name = dl_script.replace(".dl", "_souffle_cc_library"),
    src = dl_script,
    included_dl_scripts = [
        "//src/analysis/souffle:access_path.dl",
        "//src/analysis/souffle:flat_graph_ir.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
) for dl_script in ALL_DL_TEST_FILES]

[cc_test(
    name = dl_script.replace(".dl", "_test"),
    srcs = ["fact_test_driver.cc"],
    args = [
        dl_script.replace(".dl", ""),
        "invert" if dl_script in FAILURE_DL_TEST_FILES else "",
    ],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_lib",
        dl_script.replace(".dl", "_souffle_cc_library"),
    ],
) for dl_script in ALL_DL_TEST_FILES]
