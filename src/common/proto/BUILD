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
package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

# These files contain marker libraries for indicating a dependency upon
# protobuf libraries. We have to use them because the external protobuf
# library provides a single BUILD target that encompasses all protobuf
# headers. However, the Google-intern targets are much more fine grained.
alias(
    name = "protobuf",
    actual = "@com_google_protobuf//:protobuf_headers",
)

alias(
    name = "json_util",
    actual = "@com_google_protobuf//:protobuf_headers",
)

alias(
    name = "proto_message_differencer",
    actual = "@com_google_protobuf//:protobuf_headers",
)
