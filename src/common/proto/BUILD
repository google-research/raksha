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

package(default_visibility = ["//src:__subpackages__"])

licenses(["notice"])

# These files contain dummy marker libraries for indicating a dependency upon
# protobuf libraries. We have to use them because Bazel cannot be configured
# to just force explicit inclusion of protobuf dependencies. This causes
# problems when we try to import Raksha into Google's internal repository.
cc_library(
    name = "protobuf",
)

cc_library(
    name = "proto_message_differencer",
)
