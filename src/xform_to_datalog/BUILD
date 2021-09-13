licenses(["notice"])

cc_library(
    name = "manifest_datalog_facts",
    srcs = ["manifest_datalog_facts.cc"],
    hdrs = ["manifest_datalog_facts.h"],
    deps = [
        "//src/ir",
        "//src/xform_to_datalog/arcs_manifest_tree",
        "//third_party/arcs/proto:manifest_cc_proto",
    ],
)

cc_test(
    name = "manifest_datalog_facts_test",
    srcs = ["manifest_datalog_facts_test.cc"],
    deps = [
        ":manifest_datalog_facts",
        "//src/common/testing:gtest",
    ],
)
