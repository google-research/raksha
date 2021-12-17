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
#-----------------------------------------------------------------------------
load("//build_defs:arcs.bzl", "arcs_manifest_proto")
load("//build_defs:souffle.bzl", "souffle_cc_library")

def raksha_policy_check(name, src, visibility = None):
    """ Generates a cc_test rule for verifying policy compliance.

    Args:
      name: String; Name of the check.
      src: String; The `.raksha` file containing the arcs manifest describing
                   the dataflow graph and the authorization logic facts separated
                   by `// __AUTH_LOGIC__` line separator.
      visibility: List; List of visibilities.
    """
    # Split file into two '.arcs' and '.auth'.
    arcs_file = src.replace(".raksha", ".arcs")
    auth_file = src.replace(".raksha", ".auth")
    splitter_target = "%s_splitter" % name
    native.genrule(
        name = splitter_target,
        srcs = [src],
        outs = [arcs_file, auth_file],
        cmd = "csplit --prefix=part $< '/^//[ \t]*__AUTH_LOGIC__[ \t]*$$/' " +
           "&& cp part00 $(location %s) && cp part01 $(location %s)"
           % (arcs_file, auth_file)
    )
    policy_check(
        name,
        dataflow_graph = arcs_file,
        auth_logic = auth_file,
        visibility = visibility
    )

def policy_check(name, dataflow_graph, auth_logic, expect_failure = False, visibility = None):
    """ Generates a cc_test rule for verifying policy compliance.

    Args:
      name: String; Name of the check.
      dataflow_graph: String; The arcs manifest describing the dataflow graph.
      auth_logic: String; The file with authorization logic facts.
      visibility: List; List of visibilities.
    """
    # Parse .arcs into proto
    proto_target_name = "%s_proto" % name
    proto_target = ":%s" % proto_target_name
    arcs_manifest_proto(
        name = proto_target_name,
        src = dataflow_graph,
    )
    invert_arg = ""
    if expect_failure:
      invert_arg = "invert"
    # Generate datalog
    datalog_target_name = "%s_datalog" % name
    datalog_target = ":%s" % datalog_target_name
    datalog_file = "%s.dl" % name
    native.genrule(
        name = datalog_target_name,
        srcs = [
           auth_logic,
           proto_target,
        ],
        outs = [datalog_file],
        cmd = "$(location //src/xform_to_datalog:generate_datalog_program) " +
               " --auth_logic_file=\"$(location %s)\" " % auth_logic +
               " --manifest_proto=\"$(location %s)\" " % proto_target +
               " --datalog_file=\"$@\" ",
        tools = ["//src/xform_to_datalog:generate_datalog_program"],
    )
    # Generate souffle C++ library
    souffle_dl_cpp_target_name = "%s_dl_cpp" % name
    souffle_dl_cpp_target = ":%s" % souffle_dl_cpp_target_name
    souffle_cc_library(
        name = souffle_dl_cpp_target_name,
        src = datalog_target,
        included_dl_scripts = [
            "//src/analysis/souffle:authorization_logic.dl",
            "//src/analysis/souffle:dataflow_graph.dl",
            "//src/analysis/souffle:operations.dl",
            "//src/analysis/souffle:taint.dl",
            "//src/analysis/souffle:tags.dl",
            "//src/analysis/souffle:may_will.dl"
        ]
    )
    native.cc_test(
        name = name,
        srcs = ["//src/analysis/souffle/tests/arcs_fact_tests:fact_test_driver.cc"],
        args = [
            datalog_file.replace(".dl", "_datalog"),
            invert_arg
        ],
        copts = [
            "-Iexternal/souffle/src/include/souffle",
        ],
        linkopts = ["-pthread"],
        deps = [
            "@souffle//:souffle_include_lib",
            souffle_dl_cpp_target,
        ],
    )
