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


#ifndef LOW_GRAPH_SOUFFLE_EMITTER_H_
#define LOW_GRAPH_SOUFFLE_EMITTER_H_

#include <string>
#include "low_graph_ir.h"

namespace raksha_graph_ir {

class LowGraphSouffleEmitter {
    public:
        static std::string EmitSouffleFromLowGraph(LowGraph);

    private:
        static std::string EmitNode(LowGraphNode);
        static std::string EmitNodeBody(LowGraphNode);
        static std::string EmitPathList(std::vector<LowGraphAccessPath>);
};

} // raksha_graph_ir

#endif // LOW_GRAPH_SOUFFLE_EMITTER_H_

