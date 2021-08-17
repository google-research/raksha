#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_MANIFEST_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_MANIFEST_H_

#include <memory>

namespace raksha::transform::arcs_manifest_tree {

// A structure representing a single Arcs manifest. This acts as an owner
// for all substructures of an Arcs manifest and ensures that all such
// substructures live as long as the manifest. This allows substructures
// to point at each other without fear of use after free.
class Manifest {
  private:
    Manifest() {}

    // Return an owned instance of the Manifest. This is the standard
    // way to construct these objects; all constructors of this class are
    // private.
  public:
    static std::unique_ptr<Manifest> GetInstance() {
      return std::unique_ptr::make_unique<Manifest>();
    }
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_MANIFEST_H_
