#-------------------------------------------------------------------------------
# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------------

load(
    "//build_defs:souffle.bzl",
    "gen_souffle_cxx_code",
    "souffle_cc_library",
)

package(
    default_applicable_licenses = ["//third_party/raksha:license"],
    features = ["layering_check"],
    licenses = ["notice"],
)

FAILURE_DL_TEST_FILES = glob(["*_expect_fails.dl"])

ALL_DL_TEST_FILES = glob(
    ["*.dl"],
    exclude = ["*_helper.dl"],
)

TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES = ["claim_not_edge_two_inputs_two_outputs.dl"]

exports_files([
    "fact_test_driver.cc",
    "diff_wrapper.sh",
])

[gen_souffle_cxx_code(
    name = dl_script.replace(".dl", ""),
    src = dl_script,
    all_principals_own_all_tags = True,
    for_test = True,
    included_dl_scripts = [
        ":integrity_tag_prop_helper.dl",
        ":confidentiality_tag_prop_helper.dl",
        "//src/analysis/souffle:arcsjs_core.dl",
        "//src/analysis/souffle:arcsjs_check_and_claim.dl",
        "//src/analysis/souffle:arcsjs_input_and_output.dl",
        "//src/analysis/souffle:arcsjs_make_public.dl",
        "//src/analysis/souffle:arcsjs_opaque.dl",
        "//src/analysis/souffle:arcsjs_user_consent_to_downgrade.dl",
        "//src/analysis/souffle:attributes.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:check_predicate.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:operations.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:arcsjs_test_helper.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
) for dl_script in ALL_DL_TEST_FILES]

[souffle_cc_library(
    name = dl_script.replace(".dl", "_souffle_cc_library"),
    testonly = True,
    src = dl_script.replace(".dl", ""),
) for dl_script in ALL_DL_TEST_FILES]

[cc_test(
    name = dl_script.replace(".dl", "_test"),
    srcs = ["fact_test_driver.cc"],
    args = [
        dl_script.replace(".dl", ""),
        "invert" if dl_script in FAILURE_DL_TEST_FILES else "",
    ],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_include_lib",
        dl_script.replace(".dl", "_souffle_cc_library"),
    ],
) for dl_script in ALL_DL_TEST_FILES]

[gen_souffle_cxx_code(
    name = dl_script.replace(".dl", "_no_owners"),
    src = dl_script,
    all_principals_own_all_tags = False,
    for_test = True,
    included_dl_scripts = [
        ":integrity_tag_prop_helper.dl",
        "//src/analysis/souffle:attributes.dl",
        "//src/analysis/souffle:authorization_logic.dl",
        "//src/analysis/souffle:check_predicate.dl",
        "//src/analysis/souffle:dataflow_graph.dl",
        "//src/analysis/souffle:operations.dl",
        "//src/analysis/souffle:taint.dl",
        "//src/analysis/souffle:tags.dl",
        "//src/analysis/souffle:fact_test_helper.dl",
    ],
) for dl_script in TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES]

[souffle_cc_library(
    name = dl_script.replace(".dl", "_no_owners_souffle_cc_library"),
    testonly = True,
    src = dl_script.replace(".dl", "_no_owners"),
) for dl_script in TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES]

[cc_test(
    name = dl_script.replace(".dl", "_no_owners_test"),
    srcs = ["fact_test_driver.cc"],
    args = [
        dl_script.replace(".dl", "_no_owners"),
        "invert" if dl_script in FAILURE_DL_TEST_FILES else "",
    ],
    copts = [
        "-fexceptions",
        "-Iexternal/souffle/src/include/souffle",
        "-DALL_PRINCIPALS_OWN_ALL_TAGS=1",
    ],
    # Turn off header modules, as Google precompiled headers use
    # -fno-exceptions, and combining a precompiled header with -fno-exceptions
    # with a binary that uses -fexceptions makes Clang upset.
    features = ["-use_header_modules"],
    linkopts = ["-pthread"],
    deps = [
        "@souffle//:souffle_include_lib",
        dl_script.replace(".dl", "_no_owners_souffle_cc_library"),
    ],
) for dl_script in TURN_OFF_ALL_OWNERS_OWN_ALL_TAGS_FILES]
