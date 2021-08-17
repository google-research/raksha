#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_H_

#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"

#include <memory>
#include <string>

namespace raksha::transform::arcs_manifest_tree {

class Handle {
 public:

  static std::unique_ptr<Handle> Create(
      const arcs::HandleProto &handle_proto);

  absl::string_view GetName() const {
    return name_;
  }

  // Handle is neither copyable nor movable.
  Handle(const Handle &) = delete;

  Handle &operator=(const Handle &) = delete;
 private:

  const std::string name_;

  explicit Handle(const std::string name)
      : name_(name) {}
};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_H_
