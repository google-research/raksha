licenses(["notice"])

cc_library(
    name = "xform_to_datalog",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc"],
    ),
    hdrs = glob(["*.h"]),
    deps = [
        "//src/ir",
        "//src/xform_to_datalog/arcs_manifest_tree",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "datalog_facts_test",
    srcs = ["datalog_facts_test.cc"],
    deps = [
        ":xform_to_datalog",
        "//src/common/testing:gtest",
    ],
)
