load("//build_defs:raksha.bzl", "policy_check")

policy_check(
    name = "check_multimic_userc_tag_fail",
    auth_logic = "multimic.authlogic",
    dataflow_graph = "multimic.arcs",
    expect_failure = True,
)

policy_check(
    name = "check_multimic_pass",
    auth_logic = "multimic_no_userc_tag.authlogic",
    dataflow_graph = "multimic.arcs",
)

licenses(["notice"])
