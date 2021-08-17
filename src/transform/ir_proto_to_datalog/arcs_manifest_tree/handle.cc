#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/handle.h"
#include "src/transform/ir_proto_to_datalog/logging/context_logger.h"

#include <memory>

namespace raksha::transform::arcs_manifest_tree {

std::unique_ptr<Handle> Handle::Create(const arcs::HandleProto &handle_proto) {
  std::string const &handle_name = handle_proto.name();
  if (handle_name.empty()) {
    raksha::transform::logging::ContextLogger::log(
        "Handle missing required field name.");
    return std::unique_ptr<Handle>();
  }
  return std::unique_ptr<Handle>(new Handle(handle_name));
}

}  // namespace arcs_manifest_tree
