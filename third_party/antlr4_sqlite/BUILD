#-----------------------------------------------------------------------------
# Copyright 2023 Google LLC
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

load("//build_defs:antlr.bzl", "antlr4_cc_combined")

# Antlr insists that the name of the file from which it parses the parser or
# lexer should match the grammar specified in that file. To ensure this happens,
# we have these genrules that just copy the files to names that match their
# grammar specification.

genrule(
    name = "gen_sqlite_parser",
    srcs = ["@antlr4_sqlite_parser//file"],
    outs = ["SQLiteParser.g4"],
    cmd = "cp $< $@",
)

genrule(
    name = "gen_sqlite_lexer",
    srcs = ["@antlr4_sqlite_lexer//file"],
    outs = ["SQLiteLexer.g4"],
    cmd = "cp $< $@",
)

antlr4_cc_combined(
    name = "antlr4_sqlite_parser",
    src = ":SQLiteParser.g4",
    listener = False,
    token_vocab = ":SQLiteLexer.g4",
    visitor = True,
)
