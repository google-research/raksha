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

#include "low_graph_ir.h"
#include <string>
#include <vector>
#include <sstream>

namespace raksha_graph_ir {

const std::string& LowGraphAccessPath::GetName() const {
    return full_path_name;
}

const std::string& LowGraphAccessPath::PrettyPrint() const { 
    return full_path_name;
}

const std::string& LowGraphNode::GetName() const {
    return name;
}

const std::vector<LowGraphAccessPath>&
LowGraphNode::GetInputs() const {
    return inputs;
}

const std::vector<LowGraphAccessPath>&
LowGraphNode::GetOutputs() const {
    return outputs;
}

std::string LowGraphNode::PrettyPrintAPVec(
        std::vector<LowGraphAccessPath> vec) {
    std::ostringstream ss;
    bool isFirstItem = true;
    ss << "[";
    for (auto accessPath : vec) {
        if(!isFirstItem) {
            ss << ", ";
        } else {
            isFirstItem = false;
        }
        ss << accessPath.PrettyPrint();
    }
    ss << "]";
    return ss.str();
}

std::string LowGraphNode::PrettyPrint() const {
    std::ostringstream ss;
    ss << "Particle " << name << " {" << std::endl;
    ss << "    inputs: " << PrettyPrintAPVec(inputs) << std::endl;
    ss << "    outputs: " << PrettyPrintAPVec(outputs) << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

} // raksha_graph_ir
