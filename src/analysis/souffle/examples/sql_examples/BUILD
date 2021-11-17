load(
    "//build_defs:souffle.bzl",
    "souffle_cc_library",
)

licenses(["notice"])

SUCCESS_QUERIES = ["query1.dl"]
FAILURE_QUERIES = ["query3.dl"]

ALL_QUERIES = SUCCESS_QUERIES + FAILURE_QUERIES

[souffle_cc_library(
    name = query.replace(".dl", "_souffle_cc_library"),
    src = query,
    all_principals_own_all_tags = True,
    included_dl_scripts = [
        ":policy.dl",
        ":sql_aux.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
) for query in ALL_QUERIES]

[cc_test(
    name = query.replace(".dl", "_test"),
    srcs = ["//src/analysis/souffle/tests/arcs_fact_tests:fact_test_driver.cc"],
    args = [
        query.replace(".dl", ""),
        "invert" if query in FAILURE_QUERIES else "",
    ],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
    ],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_lib",
        query.replace(".dl", "_souffle_cc_library"),
    ],
) for query in ALL_QUERIES]

