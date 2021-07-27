#include "low_graph_ir.h"

#include <vector>
#include <string>
#include <iostream>

namespace raksha_graph_ir {

int main() {
    LowGraphAccessPath* p_a = new LowGraphAccessPath("a");
    LowGraphAccessPath* p_b = new LowGraphAccessPath("b");
    LowGraphAccessPath* p_c = new LowGraphAccessPath("c");
    vector<LowGraphAccessPath> inputs = {*p_a, *p_b};
    vector<LowGraphAccessPath> outputs = {*p_c};
    LowGraphNode* g = new LowGraphNode("test_node", inputs, outputs);
    cout << g->PrettyPrint() << endl;

    return 0;
}

}
