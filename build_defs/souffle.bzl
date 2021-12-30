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

def souffle_cc_library(
        name,
        src,
        all_principals_own_all_tags = False,
        included_dl_scripts = [],
        testonly = None,
        visibility = None):
    """Generates a C++ interface for the given datalog file.

    Args:
      name: String; Name of the library.
      src: String; The datalog program.
      included_dl_scripts: List; List of labels indicating datalog files included by src.
      testonly: bool; Whether the generated rules should be testonly.
      visibility: List; List of visibilities.
    """
    if all_principals_own_all_tags:
        cc_file = src + "_no_owners.cpp"
    else:
        cc_file = src + ".cpp"

    # If testonly was not explicitly set by the caller, set it based upon the
    # value of all_principals_own_all_tags. If the caller tried to explicitly
    # set all_principals_own_all_tags and set testonly to False, complain.
    if testonly == None:
        testonly = all_principals_own_all_tags
    elif (testonly == False) and (all_principals_own_all_tags):
        fail("Cannot set testonly to False and all_principals_own_all_tags to True simultaneously!")

    include_str = ""

    include_dir_opts = [
        "--include-dir=`dirname $(location {})`".format(include_file)
        for include_file in included_dl_scripts
    ]

    include_opts_str = " ".join(include_dir_opts)

    tag_ownership_str = ""
    if all_principals_own_all_tags:
        tag_ownership_str = "--macro='ALL_PRINCIPALS_OWN_ALL_TAGS=1'"

    native.genrule(
        name = name + "_cpp",
        srcs = [src] + included_dl_scripts,
        outs = [cc_file],
        testonly = testonly,
        cmd =
            "$(location @souffle//:souffle) {include_str} {tag_ownership} -g $@ $(location {src_rule})".format(include_str = include_opts_str, tag_ownership = tag_ownership_str, src_rule = src),
        tools = ["@souffle//:souffle"],
        visibility = visibility,
    )
    native.cc_library(
        name = name,
        srcs = [cc_file],
        testonly = testonly,
        copts = [
            "-Iexternal/souffle/src/include/souffle",
            "-std=c++17",
            # We didn't author this C++ file, it is generated by Souffle. We
            # don't care about non-critical issues in it. Turn off warnings.
            "-w",
        ],
        defines = [
            "__EMBEDDED_SOUFFLE__",
        ],
        deps = ["@souffle//:souffle_include_lib"],
        alwayslink = True,
        visibility = visibility,
    )
