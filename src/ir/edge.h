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
//----------------------------------------------------------------------------

#ifndef SRC_IR_EDGE_H_
#define SRC_IR_EDGE_H_

#include "src/ir/access_path.h"
#include "src/ir/datalog_print_context.h"
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
  std::string ToDatalog(DatalogPrintContext &) const {
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
