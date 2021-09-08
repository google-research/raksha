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

#ifndef SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
#define SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_

#include <vector>

#include "absl/strings/str_format.h"
#include "src/ir/edge.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog {

class DatalogFacts {
 public:
  static DatalogFacts CreateFromManifestProto(
      const arcs::ManifestProto &manifest_proto);

  DatalogFacts(
      std::vector<raksha::ir::TagClaim> claims,
      std::vector<raksha::ir::TagCheck> checks,
      std::vector<raksha::ir::Edge> edges)
      : claims_(std::move(claims)), checks_(std::move(checks)),
        edges_(std::move(edges)) {}

  // Print out all contained facts as a single datalog string. Note: this
  // does not contain the header files that would be necessary to run this
  // against the datalog scripts; it contains only facts and comments.
  std::string ToString() const {
    auto tostring_formatter = [](std::string *out, const auto &arg) {
      out->append(arg.ToString()); };

    return absl::StrFormat(
        kFactOutputFormat,
        absl::StrJoin(claims_, "", tostring_formatter),
        absl::StrJoin(checks_, "", tostring_formatter),
        absl::StrJoin(edges_, "", tostring_formatter));
  }

  const std::vector<raksha::ir::TagClaim> &claims() const { return claims_; }

  const std::vector<raksha::ir::TagCheck> &checks() const { return checks_; }

  const std::vector<raksha::ir::Edge> &edges() const { return edges_; }

 private:
  static constexpr absl::string_view kFactOutputFormat = R"FORMAT(
// Claims:
%s
// Checks:
%s
// Edges:
%s)FORMAT";

  std::vector<raksha::ir::TagClaim> claims_;
  std::vector<raksha::ir::TagCheck> checks_;
  std::vector<raksha::ir::Edge> edges_;
};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
