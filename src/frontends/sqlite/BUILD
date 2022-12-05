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

# This directory contains a Raksha IR frontend for sqlite.

package(
    default_visibility = ["//src:__subpackages__"],
    licenses = ["notice"],
)

cc_library(
    name = "raksha_sqlite_c_interface",
    hdrs = ["raksha_sqlite_c_interface.h"],
    visibility = ["//visibility:public"],
)

# Note: in order for -D RAKSHA_FRONTEND (from the defines argument) to work,
# this needs to be re-built from the source and header files rather than
# pulling in //./sqlite_fork:raksha_sqlite_lib and adding the defines argument
# on top of that. If the sources are just pulled in as a dependency, it will
# not build them with the defines flag.
cc_library(
    name = "raksha_snooping_sqlite_lib",
    srcs = [
        "//src/frontends/sqlite/sqlite_fork:sqlite3_github.c",
    ],
    hdrs = [
        "raksha_sqlite_c_interface.h",
        "//src/frontends/sqlite/sqlite_fork:sqlite3.h",
    ],
    defines = ["RAKSHA_FRONTEND"],
)

cc_binary(
    name = "raksha_snooping_sqlite_cmd",
    srcs = [
        "//src/frontends/sqlite/sqlite_fork:shell_github.c",
        "//src/frontends/sqlite/sqlite_fork:sqlite3.h",
    ],
    copts = [
        "-DHAVE_READLINE=1",
    ],
    defines = ["RAKSHA_FRONTEND"],
    linkopts = ["-ldl"],
    deps = [
        ":raksha_snooping_sqlite_lib",
        "//third_party/libedit:pretend_to_be_gnu_readline_system",
    ],
)
