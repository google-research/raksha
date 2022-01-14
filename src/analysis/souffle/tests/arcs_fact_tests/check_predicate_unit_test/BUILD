load(
    "//build_defs:test_helpers.bzl",
    "run_taint_exec_compare_check_results"
)

run_taint_exec_compare_check_results(
    name = "check_pred_unit_test",
    check_file = "checkAndExpectation.facts",
    other_input_files = glob(["*.facts"], exclude=["*checkAndExpectation.facts"]),
)
