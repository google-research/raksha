//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include "low_graph_souffle_emitter.h"

using namespace std;

string LowGraphSouffleEmitter::EmitPathList(
        vector<LowGraphAccessPath> pathList) {
    string ret = "$Nil()";
    for (auto path: pathList) {
        ret = "$Cons(" + path.full_path_name + ", " + ret + ")";
    }
    return ret;
};

string LowGraphSouffleEmitter::EmitNodeBody(LowGraphNode node) {
    return "$FlatParticle(" + EmitPathList(node.inputs) + ", " +
        EmitPathList(node.outputs) + ")";
}

string LowGraphSouffleEmitter::EmitNode(LowGraphNode node) {
    return "bindFlatParticle(" + node.name + ", " +
        EmitNodeBody(node) + ").";
}

string LowGraphSouffleEmitter::EmitSouffleFromLowGraph(LowGraph low_graph) {
    string ret = "";
    for (auto node : low_graph) {
        ret = ret += EmitNode(node) + "\n\n";
    }
    return ret;
}
