#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
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
"""
This module genrates a cc_library rule for policy verifier
"""

load("//build_defs:souffle.bzl", "gen_souffle_cxx_code", "souffle_cc_library")
load(
    "//src/analysis/souffle:dl_file_lists.bzl",
    "policy_verifier_include_dl_files",
)

def raksha_policy_verifier_library(
        name,
        policies,
        visibility = None,
        policy_verifier_interfaces = ["//src/analysis/souffle:policy_verifier_interface.dl"],
        additional_dl_files = [],
        additional_souffle_cc_lib_dependencies = []):
    """Generates corresponding datalog file for each policy.

    Args:
      name: Name of the check
      policies: list of policies under a single name.
      visibility: List of visibilities
      policy_verifier_interfaces: list; policy verifier interface target.
      additional_dl_files: list; list of additional dl file targets.
      additional_souffle_cc_lib_dependencies: list; list of all souffle cc library dependencies.
    """

    auth_logic_file = "%s_combined_auth_logic" % name
    concat_all_auth_logic_target_name = "%s_concat_all_auth" % name
    native.genrule(
        name = concat_all_auth_logic_target_name,
        srcs = policies,
        outs = [auth_logic_file],
        cmd = "cat $(SRCS) > $@",
    )
    auth_logic_files = [auth_logic_file]
    policy_library(
        name,
        auth_logic_files,
        visibility,
        policy_verifier_interfaces,
        additional_dl_files,
        additional_souffle_cc_lib_dependencies,
    )

def policy_library(
        name,
        auth_logic_files,
        visibility = None,
        policy_verifier_interfaces = ["//src/analysis/souffle:policy_verifier_interface.dl"],
        additional_dl_files = [],
        additional_souffle_cc_lib_dependencies = []):
    """ Generates a cc_library rule for verifying policy compliance.

    Args:
      name: String; Name of the check
      auth_logic_files: list; The file with authorization logic facts.
      visibility: List of visibilities.
      policy_verifier_interfaces: list; policy verifier interface target.
      additional_dl_files: list; list of additional dl file targets.
      additional_souffle_cc_lib_dependencies: list; list of all souffle cc library dependencies.
    """

    #TODO (b/232451284) Fix auth_logic_files to be a string. We had to turn auth_logic_files
    #and policy_verifier_interface into a list rather than a string to evade the bug mentioned.
    auth_logic_file = auth_logic_files[0]
    policy_verifier_interface = policy_verifier_interfaces[0]

    # Generate datalog
    datalog_target_name = "%s_verifier_interface_datalog" % name
    datalog_source_target_name = "%s_datalog" % name
    datalog_library_target_name = "%s" % name
    datalog_cxx_source_target_name = "%s_cxx" % name
    datalog_cxx_source_target = ":%s" % datalog_cxx_source_target_name

    generated_datalog_file_from_auth_logic = "%s_auth_logic.dl" % name
    generated_datalog_file_from_policy_verifier_interface_and_auth_logic = "%s_policy_interface_auth_logic.dl" % name

    native.genrule(
        name = datalog_source_target_name,
        srcs = [
            auth_logic_file,
        ],
        outs = [generated_datalog_file_from_auth_logic],
        cmd = "$(location //src/backends/policy_engine/souffle:raksha_policy_datalog_emitter) " +
              " --policy_rules=\"$(location %s)\" " % auth_logic_file +
              " --datalog_file=\"$@\" ",
        tools = ["//src/backends/policy_engine/souffle:raksha_policy_datalog_emitter"],
        visibility = visibility,
    )

    native.genrule(
        name = datalog_target_name,
        srcs = [
            generated_datalog_file_from_auth_logic,
            policy_verifier_interface,
        ],
        outs = [generated_datalog_file_from_policy_verifier_interface_and_auth_logic],
        cmd = "cat $(location %s) $(location %s) > $@" % (policy_verifier_interface, generated_datalog_file_from_auth_logic),
        visibility = visibility,
    )

    # Generate souffle C++ library
    gen_souffle_cxx_code(
        name = datalog_cxx_source_target_name,
        src = generated_datalog_file_from_policy_verifier_interface_and_auth_logic,
        included_dl_scripts = policy_verifier_include_dl_files + additional_dl_files,
        visibility = visibility,
    )
    souffle_cc_library(
        name = datalog_library_target_name,
        src = datalog_cxx_source_target,
        additional_deps = additional_souffle_cc_lib_dependencies,
        visibility = visibility,
    )
