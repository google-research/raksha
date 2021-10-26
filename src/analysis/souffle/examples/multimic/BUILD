load("//build_defs:raksha.bzl", "policy_check")

policy_check("check_multimic_userc_tag_fail", "multimic.arcs", "multimic.authlogic", True)
policy_check("check_multimic_pass", "multimic.arcs", "multimic_no_userc_tag.authlogic")
policy_check("check_multimic_no_downgrade_fail", "multimic.arcs", "multimic_no_userc_tag_no_miccompute_downgrade.authlogic", True)
