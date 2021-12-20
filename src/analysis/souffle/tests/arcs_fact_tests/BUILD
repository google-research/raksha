load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

FAILURE_DL_TEST_FILES = glob(["*_expect_fails.dl"])

ALL_DL_TEST_FILES = glob(["*.dl"], exclude = ["*_helper.dl"])

TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES = ["claim_not_edge_two_inputs_two_outputs.dl"]

exports_files(["fact_test_driver.cc"])

[souffle_cc_library(
    name = dl_script.replace(".dl", "_souffle_cc_library"),
    src = dl_script,
    all_principals_own_all_tags = True,
    included_dl_scripts = [
        ":integrity_tag_prop_helper.dl",
        ":confidentiality_tag_prop_helper.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:operations.dl",
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
    ],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_include_lib",
        dl_script.replace(".dl", "_souffle_cc_library"),
    ],
) for dl_script in ALL_DL_TEST_FILES]

[souffle_cc_library(
    name = dl_script.replace(".dl", "_no_owners_souffle_cc_library"),
    src = dl_script,
    all_principals_own_all_tags = False,
    included_dl_scripts = [
        ":integrity_tag_prop_helper.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:operations.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
) for dl_script in TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES]

[cc_test(
    name = dl_script.replace(".dl", "_no_owners_test"),
    srcs = ["fact_test_driver.cc"],
    args = [
        dl_script.replace(".dl", ""),
        "invert" if dl_script in FAILURE_DL_TEST_FILES else "",
    ],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-DALL_PRINCIPALS_OWN_ALL_TAGS=1",
    ],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_include_lib",
        dl_script.replace(".dl", "_no_owners_souffle_cc_library"),
    ],
) for dl_script in TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES]
