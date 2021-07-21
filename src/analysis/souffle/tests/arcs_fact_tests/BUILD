load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

FAILURE_DL_TEST_FILES = glob(["*_expect_fails.dl"])

ALL_DL_TEST_FILES = glob(["*.dl"])

[souffle_cc_library(
    name = dl_script.replace(".dl", "_souffle_cc_library"),
    included_dl_scripts = [
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
    src = dl_script,
) for dl_script in ALL_DL_TEST_FILES]

[cc_test(
    name = dl_script.replace(".dl", "_test"),
    srcs = ["fact_test_driver.cc"],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    deps = [
        "@souffle//:souffle_lib",
        dl_script.replace(".dl", "_souffle_cc_library"),
    ],
    linkopts = ["-pthread"],
    args = [
        dl_script.replace(".dl", ""),
        "invert" if dl_script in FAILURE_DL_TEST_FILES else ""
    ],
) for dl_script in ALL_DL_TEST_FILES]

test_suite(
    name = "arcs_fact_tests",
    tests = [dl_script.replace(".dl", "_test")
             for dl_script in ALL_DL_TEST_FILES]
)
