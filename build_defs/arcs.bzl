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
def arcs_manifest_proto(name, src, deps = [], visibility = None):
    """ Parses an `.arcs` file and generates a binary protobuf file "name.binarpb".

    Args:
      name: String; Name of the library.
      src: String; The .arcs manifest file.
      deps: List of targets; dependent source files.
      visibility: List; List of visibilities.
    """
    tool = "//third_party/arcs/tools/manifest2proto"
    out = name + ".binarypb"
    args = "--quiet --outdir $$(dirname $@) --outfile $$(basename $@) $(location {src})".format(src = src)
    cmd = "$(location %s) %s" % (tool, args)
    native.genrule(
        name = name,
        srcs = [src] + deps,
        outs = [out],
        cmd = cmd,
        tools = [tool],
    )
