#ifndef SRC_IR_EDGE_H_
#define SRC_IR_EDGE_H_

#include "src/ir/access_path.h"
#include "absl/strings/str_format.h"

namespace raksha::ir {

// A directed dataflow edge from AccessPath from_ to AccessPath to_. Abstract
// data, such as tags, flow along these edges in the dataflow graph we
// present to the analyzer.
class Edge {
 public:
  explicit Edge(AccessPath from, AccessPath to)
    : from_(std::move(from)), to_(std::move(to)) {}

  // Print the edge as a string containing a Datalog fact.
  std::string ToString() const {
    constexpr absl::string_view kEdgeFormat = R"(edge("%s", "%s").)";
    return absl::StrFormat(kEdgeFormat, from_.ToString(), to_.ToString());
  }

  bool operator==(const Edge &other) const {
    return (from_ == other.from_) && (to_ == other.to_);
  }

  Edge BulkInstantiate(
      const absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
          &instantiation_map) const {
    return Edge(
        from_.BulkInstantiate(instantiation_map),
        to_.BulkInstantiate(instantiation_map));
  }

 private:
  // The AccessPath we are drawing the edge from.
  AccessPath from_;
  // The AccessPath we are drawing the edge to.
  AccessPath to_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_EDGE_H_
