load("//build_defs:souffle.bzl", "find_failing_tag_checks_test")

licenses(["notice"])

find_failing_tag_checks_test(
    name = "fail_different_tag_test",
    edge_facts_file=":edge.facts",
    claim_has_tag_facts=":claim_has_tag.facts",
    check_has_tag_facts=":check_has_tag.facts",
    expected_output=":expected_output.txt")
