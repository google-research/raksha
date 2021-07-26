#include "low_graph_ir.h"
#include "low_graph_souffle_emitter.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

LowGraph MakeTestGraph() {
    // Particle 1: ImageDetector
    //      reads: 
    //          sensor_data_packet { camera_feed, video_resolution }
    //          image_detection_model
    //      writes:
    //          image_detection_boxes
    
    // Particle 2: ImageSelector
    //     reads:
    //         image_detection_boxes
    //         image_selection   // image selection action from user
    //     writes:
    //         selected_image_id

    // Particle 1
    vector<LowGraphAccessPath> image_detector_inputs = {
        *(new LowGraphAccessPath("sensor_data_packet.camera_feed")),
        *(new LowGraphAccessPath("sensor_data_packet.video_resolution")),
        *(new LowGraphAccessPath("image_detection_model"))
    };
    vector<LowGraphAccessPath> image_detector_outputs = {
        *(new LowGraphAccessPath("image_detection_boxes"))
    };
    LowGraphNode* image_detector = new LowGraphNode("ImageDetector",
            image_detector_inputs, image_detector_outputs);

    // Particle 2
    vector<LowGraphAccessPath> image_selector_inputs = {
        *(new LowGraphAccessPath("image_detection_boxes")),
        *(new LowGraphAccessPath("image_selection"))
    };
    vector<LowGraphAccessPath> image_selector_outputs = {
        *(new LowGraphAccessPath("selected_image_id"))
    };
    LowGraphNode* image_selector = new LowGraphNode("ImageSelector",
        image_selector_inputs, image_selector_outputs);

    LowGraph graph = {*image_detector, *image_selector};
    return graph;
}

int main() {
    LowGraph graph = MakeTestGraph();
    cout << LowGraphSouffleEmitter::EmitSouffleFromLowGraph(graph) << endl;
}
