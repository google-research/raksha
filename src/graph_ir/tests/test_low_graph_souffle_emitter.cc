#include "low_graph_ir.h"
#include "low_graph_souffle_emitter.h"

#include <vector>
#include <string>
#include <iostream>

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

namespace raksha_graph_ir {
// Particle 1
static const std::vector<LowGraphAccessPath> image_detector_inputs = {
    LowGraphAccessPath("sensor_data_packet.camera_feed"),
    LowGraphAccessPath("sensor_data_packet.video_resolution"),
    LowGraphAccessPath("image_detection_model")
};
static const std::vector<LowGraphAccessPath> image_detector_outputs = {
    LowGraphAccessPath("image_detection_boxes")
};
static const LowGraphNode image_detector = LowGraphNode("ImageDetector",
        image_detector_inputs, image_detector_outputs);

// Particle 2
static const std::vector<LowGraphAccessPath> image_selector_inputs = {
    LowGraphAccessPath("image_detection_boxes"),
    LowGraphAccessPath("image_selection")
};
static const std::vector<LowGraphAccessPath> image_selector_outputs = {
    LowGraphAccessPath("selected_image_id")
};
static const LowGraphNode image_selector = LowGraphNode("ImageSelector",
    image_selector_inputs, image_selector_outputs);

static const LowGraph test_graph = {image_detector, image_selector};
}

int main() {
    std::cout << raksha_graph_ir::LowGraphSouffleEmitter
            ::EmitSouffleFromLowGraph(raksha_graph_ir::test_graph)
        << std::endl;
}
