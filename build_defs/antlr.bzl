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
"""
Generate C++ parser and lexer from a grammar file.
"""

load("@rules_antlr//antlr:antlr4.bzl", "antlr")

def antlr_cc_library(name, grammar_file, package = None, listener = True, visitor = True):
    """Creates a C++ lexer and parser from a source grammar.
    Args:
      name: Base name for the lexer and the parser rules.
      src: source ANTLR grammar file
      package: The namespace for the generated code
      listener: generate ANTLR listener (default: True)
      visitor: generate ANTLR visitor (default: True)
    """
    generated = name + "_grammar"
    antlr(
        name = generated,
        srcs = [grammar_file],
        language = "Cpp",
        listener = listener,
        visitor = visitor,
        package = "",
        
    )
    
    native.cc_library(
        name = name + "_cc_parser",
        srcs = [generated],
        copts = ["-fexceptions",],
        linkopts = ["-fexceptions",],
        features = ["-use_header_modules"],
        deps = [
            generated,
            "@antlr4_runtimes//:cpp",
        ],
        alwayslink = True,
        linkstatic = 1,
    )

    # native.genrule(
    #     #cp command for all the lexer, parser, visitor generated and then add them to gitignore
    # )

def authorization_logic_datalog_generator(fileName = "canActAs", inputDir = "src/ir/auth_logic/raksha_examples", outputDir = "src/ir/auth_logic/raksha_examples"):
    native.genrule(
        name = "authorization_logic_datalog_file",
        srcs = [
            fileName,
        ],
        outs = ["datalog_auth_file"],
        cmd = "$(location //src/ir:generate_auth_datalog_program) " +
              " --file_name=\"$(location %s)\" " % fileName +
              " --input_dir=\"$(location %s)\" " % inputDir +
              " --output_dir=\"$@\" ",
        tools = ["//src/ir:generate_auth_datalog_program"],
    )

