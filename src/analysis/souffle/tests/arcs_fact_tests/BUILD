load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

DL_TEST_FILES = glob(["*.dl"])

filegroup(
    name = "test_dl_scripts",
    srcs = DL_TEST_FILES
)

[souffle_cc_library(
    name = dl_script.replace(".dl", "_souffle_cc_library"),
    included_dl_scripts = [
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
    src = dl_script,
) for dl_script in DL_TEST_FILES]

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
    args = [dl_script.replace(".dl", "")],
) for dl_script in DL_TEST_FILES]

test_suite(
    name = "arcs_fact_tests",
    tests = [dl_script.replace(".dl", "_test") for dl_script in DL_TEST_FILES],
)
