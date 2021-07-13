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

def souffle_cc_library(name, src, visibility = None):
    """Generates a C++ interface for the given datalog file.

    Args:
      name: String; Name of the library.
      src: String; The datalog program.
      visibility: List; List of visibilities.
    """
    cc_file = src + ".cpp"
    native.genrule(
        name = name + "_cpp",
        srcs = [src],
        outs = [cc_file],
        cmd = "$(location @souffle//:souffle) -g $@ $<",
        tools = ["@souffle//:souffle"],
        visibility = visibility,
    )
    native.cc_library(
        name = name,
        srcs = [cc_file],
        copts = [
            "-Iexternal/souffle/src/include/souffle",
            "-std=c++17",
        ],
        defines = [
            "__EMBEDDED_SOUFFLE__",
        ],
        deps = ["@souffle//:souffle_lib"],
        alwayslink = True,
        visibility = visibility,
    )

# The following function is heavily influenced by the example of diff_test,
# found at:
#
# https://github.com/bazelbuild/bazel-skylib/blob/main/rules/diff_test.bzl
#
# which is also owned by Google.
def _find_failing_tag_checks_test_impl(ctx):
  test_sh = ctx.actions.declare_file(ctx.label.name + "-tag-checks-test.sh")
  # This is a bit of a hack: we expect all of the facts files to be in the same
  # directory, so grab the directory of the edges fact file.
  facts_dir = ctx.file.edge_facts_file.dirname
  dl_file = ctx.file.dl_file
    
  ctx.actions.write(
      output = test_sh,
      content = r"""#!/bin/bash
set -euo pipefail
SOUFFLE_BIN="{souffle_bin}"
FACTS_DIR="{facts_dir}"
DL_FILE="{dl_file}"
EXPECTED_OUTPUT_FILE="{expected_output_file}"
if ! "$SOUFFLE_BIN" --fact-dir="$FACTS_DIR" "$DL_FILE" --output-dir=- | diff "$EXPECTED_OUTPUT_FILE" -; then
  echo >&2 "FAIL: expected output differs from observed output for tag checks."
  exit 1
fi
""".format(
        souffle_bin = ctx.executable._souffle_binary.root.path + "/" + ctx.executable._souffle_binary.path,
        dl_file=dl_file.path,
        facts_dir = facts_dir,
        expected_output_file = ctx.file.expected_output.path
      ),
      is_executable = True,
  )
  return DefaultInfo(
      executable = test_sh,
      files = depset(direct = [test_sh]),
      runfiles = ctx.runfiles(
          files = [
              test_sh,
              ctx.file.edge_facts_file,
              ctx.file.claim_has_tag_facts,
              ctx.file.check_has_tag_facts,
              ctx.file.dl_file,
              ctx.file.expected_output,
              ctx.executable._souffle_binary,
           ])
      )
      

_find_failing_tag_checks_test = rule(
    implementation = _find_failing_tag_checks_test_impl,
    test = True,
    executable = True,
    attrs = {
        "_souffle_binary": attr.label(
            allow_single_file = True,
            cfg = "target",
            default = "@souffle//:souffle",
            executable = True,
        ),
        "dl_file": attr.label(
            allow_single_file = True,
            default = "//src/analysis/souffle:find_failing_tag_checks.dl"
        ),
        "edge_facts_file": attr.label(
            allow_single_file = True,
        ),
        "claim_has_tag_facts": attr.label(
            allow_single_file = True,
        ),
        "check_has_tag_facts": attr.label(
            allow_single_file = True,
        ),
        "expected_output": attr.label(
            allow_single_file = True,
        ),
    }
  )

def find_failing_tag_checks_test(
    name,
    expected_output=":expected_output.txt",
    edge_facts_file=":edge.facts",
    claim_has_tag_facts=":claim_has_tag.facts",
    check_has_tag_facts=":check_has_tag.facts",
    ):
  package_name = native.package_name()
  dl_file_label = "//src/analysis/souffle:find_failing_tag_checks.dl"
  souffle_bin_label = "@souffle//:souffle"
  native.sh_test(
      name = name,
      srcs = ["//arcs_fact_tests:find_failing_tag_checks_test.sh"],
      args = [
          "$(location {})".format(souffle_bin_label), # SOUFFLE_BIN
          "$(location {})".format(edge_facts_file), # FACTS_DIR
          "$(location {})".format(dl_file_label), # DL_FILE
          "$(location {})".format(expected_output)
          ],
      data = [
          souffle_bin_label,
          edge_facts_file,
          claim_has_tag_facts,
          check_has_tag_facts,
          expected_output,
          dl_file_label,
          ]
      )

def no_failing_tag_checks_test(
    name,
    edge_facts_file=":edge.facts",
    claim_has_tag_facts=":claim_has_tag.facts",
    check_has_tag_facts=":check_has_tag.facts"):
  find_failing_tag_checks_test(
      name=name,
      expected_output="//arcs_fact_tests:no_failures.txt",
      edge_facts_file=edge_facts_file,
      claim_has_tag_facts=claim_has_tag_facts,
      check_has_tag_facts=check_has_tag_facts)
