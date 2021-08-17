#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_

#include "type.h"
#include "third_party/arcs/proto/manifest.pb.h"

#include "absl/status/statusor.h"

#include <string>

namespace raksha::transform::arcs_manifest_tree {

class HandleConnectionSpec {
 public:
  static absl::StatusOr<HandleConnectionSpec> Create(
      const arcs::HandleConnectionSpecProto &handle_connection_spec_proto);

  std::string const &name() {
    return name_;
  }

 private:
  HandleConnectionSpec(
    const std::string name,
    const bool reads,
    const bool writes,
    const Type type) : name_(name), reads_(reads), writes_(writes), type_(type)
    {}

  std::string name_;
  bool reads_;
  bool writes_;
  Type type_;


};

}  // namespace raksha::transform::arcs_manifest_tree

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_ARCS_MANIFEST_TREE_HANDLE_CONNECTION_SPEC_H_
