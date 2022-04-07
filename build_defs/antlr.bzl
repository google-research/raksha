#-----------------------------------------------------------------------------
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-----------------------------------------------------------------------------
"""
Generates C++ parser, visitor, listener, lexer from a grammar file.
"""

load("@rules_antlr//antlr:antlr4.bzl", "antlr")

def antlr4_cc_combined(name, src, listener = False, visitor = True):
    """Creates a C++ lexer, visitor, listener, parser from a source grammar.
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
        srcs = [src],
        language = "Cpp",
        listener = listener,
        visitor = visitor,
        package = name,
    )

    native.cc_library(
        name = name,
        srcs = [generated],
        hdrs = [generated],
        copts = ["-fexceptions"],
        linkopts = ["-fexceptions"],
        features = ["-use_header_modules"],
        alwayslink = True,
        linkstatic = 1,
        deps = [
            generated,
            "@antlr4_runtimes//:antlr_runtime_build",
        ],
        visibility = ["//src:__subpackages__"],
    )
