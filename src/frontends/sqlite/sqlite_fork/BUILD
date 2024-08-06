#-------------------------------------------------------------------------------
# Copyright 2022 Google LLC
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

# This directory contains a fork of the sqlite amalgamation which we have
# modified to build a frotnend for sqlite for Raksha. It is likely that in the
# future we will attempt to delete this and replace the changes here with define
# guards in the internal fork of sqlite.

package(
    default_visibility = ["//src:__subpackages__"],
    licenses = ["notice"],
)

cc_library(
    name = "raksha_sqlite_lib",
    srcs = ["sqlite3_github.c"],
    hdrs = ["sqlite3.h"],
)

# TODO(aferr) This build target should be deprecated once
# we can properly test calling the SELECT snoop function calls.
# It exists temporarily to demo snooping on selects when running the shell.
cc_binary(
    name = "raksha_sqlite_cmd",
    srcs = [
        "shell_github.c",
    ],
    copts = [
        "-DHAVE_READLINE=1",
    ],
    linkopts = ["-ldl"],
    deps = [
        ":raksha_sqlite_lib",
        "//third_party/libedit:pretend_to_be_gnu_readline_system",
    ],
)
