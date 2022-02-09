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

load("@buildtools//buildifier:def.bzl", "buildifier")
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

licenses(["notice"])

# Target to fix all bazel files in Raksha repo.
buildifier(
    name = "buildifier",
)

# Generates `compile_commands.json` for use with clangd.
#
# Run `bazel run //:refresh_compile_commands` to generate.
#
refresh_compile_commands(
    name = "refresh_compile_commands",

    # Specify the targets of interest.
    # For example, specify a dict of targets and their arguments:
    targets = {
        "//src/...": "",
    },
    # For more details, feel free to look into the following bzl file:
    #   `external/hedron_compile_commands/refresh_compile_commands.bzl`.
)
