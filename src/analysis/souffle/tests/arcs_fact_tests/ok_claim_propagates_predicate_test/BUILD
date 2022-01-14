load(
    "//build_defs:test_helpers.bzl",
    "run_taint_exec_compare_check_results"
)

run_taint_exec_compare_check_results(
    name = "ok_claim_propagates",
    check_file = ":checkP.facts",
    other_input_files = glob(["*.facts"], exclude=["*checkP.facts"]),
)
