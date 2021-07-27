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
#include <sstream>
#include "low_graph_souffle_emitter.h"

namespace raksha_graph_ir {

std::string LowGraphSouffleEmitter::EmitPathList(
        std::vector<LowGraphAccessPath> pathList) {
    std::string ret = "$Nil()";
    for (auto path: pathList) {
        ret = "$Cons(" + path.PrettyPrint() + ", " + ret + ")";
    }
    return ret;
};

std::string LowGraphSouffleEmitter::EmitNodeBody(LowGraphNode node) {
    return "$FlatParticle(" + EmitPathList(node.GetInputs()) + ", " +
        EmitPathList(node.GetOutputs()) + ")";
}

std::string LowGraphSouffleEmitter::EmitNode(LowGraphNode node) {
    return "bindFlatParticle(" + node.GetName() + ", " +
        EmitNodeBody(node) + ").";
}

std::string LowGraphSouffleEmitter::EmitSouffleFromLowGraph(
        LowGraph low_graph) {
    std::ostringstream ss;
    for (auto node : low_graph) {
        ss << EmitNode(node) << std::endl << std::endl;
    }
    return ss.str();
}

} // raksha_graph_ir
