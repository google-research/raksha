# Copyright 2019 Google LLC
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
# Copy of https://github.com/kythe/kythe/blob/a367ca6f/third_party/libffi.BUILD
#
load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)

configure_make(
    name = "libffi",
    configure_options = [
        "--disable-multi-os-directory",
        "--disable-dependency-tracking",
        "--disable-docs",
        "AR=ar",
    ],
    lib_source = ":all_srcs",
    visibility = ["//visibility:public"],
    alwayslink = True,
)
