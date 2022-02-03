#-----------------------------------------------------------------------------
# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#----------------------------------------------------------------------------

load("//build_defs:souffle.bzl", "gen_souffle_cxx_code", "souffle_cc_library")

def extracted_datalog_string_test(
        name,
        dl_string_lib,
        visibility = None):
    """Given a C++ library containing test data that contains Datalog strings
    and a function GatherDatalogStrings to collect those into a vector,
    produce a datalog script containing those rule bodies that should be
    parseable if the strings are parseable.

    Args:
      name: String; Name of the library.
      dl_string_lib: String; The library containing the strings to extract.
      visibility: List; List of visibilities.
    """

    native.cc_binary(
        name = "dl_string_test_file_generator_for_" + name,
        testonly = True,
        srcs = ["//src/test_utils/dl_string_extractor:dl_string_test_file_generator.cc"],
        copts = ["-std=c++17", "-fexceptions"],
        deps = [
            dl_string_lib,
            "//src/common/logging:logging",
            "//src/test_utils/dl_string_extractor:datalog_string",
        ],
    )

    native.genrule(
        name = name + "_dl",
        outs = [name + ".dl"],
        testonly = True,
        cmd =
            "$(location dl_string_test_file_generator_for_{name}) $@".format(name = name),
        tools = ["dl_string_test_file_generator_for_" + name],
        visibility = visibility,
    )

    gen_souffle_cxx_code(
        name = name + "_cxx",
        src = name + "_dl",
        for_test = True,
        included_dl_scripts = [
            "//src/analysis/souffle:authorization_logic.dl",
            "//src/analysis/souffle:check_predicate.dl",
            "//src/analysis/souffle:dataflow_graph.dl",
            "//src/analysis/souffle:operations.dl",
            "//src/analysis/souffle:taint.dl",
            "//src/analysis/souffle:tags.dl",
            "//src/analysis/souffle:fact_test_helper.dl",
        ],
    )

    souffle_cc_library(
        name = name + "_souffle_lib",
        src = name + "_cxx",
        testonly = True,
    )

    native.cc_test(
        name = name,
        srcs = ["//src/test_utils/dl_string_extractor:dl_string_parsing_test_driver.cc"],
        args = [
            name + "_cxx",
        ],
        copts = [
            "-fexceptions",
            "-Iexternal/souffle/src/include/souffle",
        ],
        linkopts = ["-pthread"],
        deps = [
            name + "_souffle_lib",
            "@souffle//:souffle_include_lib",
        ],
    )

def run_taint_exec_compare_check_results(
        name,
        input_files,
        visibility = None):
    """Run the Souffle-generated taint executable and ensure that the failing
    check output file is empty.

    Args:
      name: String; Name of the test.
      input_files: List; List of labels indicatinginput files to taint_exec.
    """

    facts_dir_opts = [
        "--facts=`dirname $(location {})`".format(input_file)
        for input_file in input_files
    ]

    # Run the taint analysis Souffle binary to generate the Datalog output
    # files, then copy the checkAndResult and expectedCheckAndResult output
    # files to the rule outputs.
    native.genrule(
        name = name + "_test_results",
        outs = ["checkAndResult", "expectedCheckAndResult"],
        srcs = input_files,
        testonly = True,
        cmd = ("$(location //src/analysis/souffle:taint_exec_test) --output=$(RULEDIR) {fact_dirs} && " +
               "cp $(RULEDIR)/checkAndResult.csv $(location :checkAndResult) && " +
               "cp $(RULEDIR)/expectedCheckAndResult.csv $(location :expectedCheckAndResult)")
            .format(fact_dirs = " ".join(facts_dir_opts)),
        tools = ["//src/analysis/souffle:taint_exec_test"],
        visibility = visibility,
    )

    # Sort the checkAndResult contents. Because the check name is the first
    # field, this should sort them by name.
    native.genrule(
        name = name + "_check_and_result_sorted",
        outs = ["checkAndResultSorted"],
        srcs = [":checkAndResult"],
        testonly = True,
        cmd = "sort $< > $@",
        visibility = visibility,
    )

    # Similarly, sort expectedCheckAndResult.
    native.genrule(
        name = name + "_expected_check_and_result_sorted",
        outs = ["expectedCheckAndResultSorted"],
        srcs = [":expectedCheckAndResult"],
        testonly = True,
        cmd = "sort $< > $@",
        visibility = visibility,
    )

    # Compare the two files for diff equality.
    native.sh_test(
        name = name,
        args = ["$(location :expectedCheckAndResultSorted)", "$(location :checkAndResultSorted)"],
        data = [":checkAndResultSorted", ":expectedCheckAndResultSorted"],
        srcs = ["//src/analysis/souffle/tests/arcs_fact_tests:diff_wrapper.sh"],
    )
