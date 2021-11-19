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

#ifndef SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_
#define SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_

#include <vector>

#include "absl/strings/str_format.h"
#include "src/ir/datalog_print_context.h"
#include "src/ir/edge.h"
#include "src/ir/particle_spec.h"
#include "src/ir/system_spec.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog {

class ManifestDatalogFacts {
 public:
  // A hacky class with just enough information about a particle instances for
  // the purpose of datalog generation. Specifically, instantiation_map is
  // passed to the `ToDatalog` methods. When the data structures are all fleshed
  // out, this class will no longer be needed and therefore, can be
  // removed. Further, datalog generation would be implemented as a visitor
  // where the instance of a particle will be clear from the visit context.
  class Particle {
   public:
    Particle(const raksha::ir::ParticleSpec *spec,
             absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>
                 &&instantiation_map,
             std::vector<ir::Edge> &&edges)
        : spec_(spec),
          instantiation_map_(instantiation_map),
          edges_(edges) {}

    const raksha::ir::ParticleSpec *spec() const { return spec_; }
    const absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>&
        instantiation_map() const { return instantiation_map_; }
    const std::vector<ir::Edge> &edges() const { return edges_; }

   private:
    const raksha::ir::ParticleSpec *spec_;
    absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>
        instantiation_map_;
    std::vector<ir::Edge> edges_;
  };

  static ManifestDatalogFacts CreateFromManifestProto(
      const ir::SystemSpec& system_spec,
      const arcs::ManifestProto &manifest_proto);

  // A default constructor creates a sensible, legal state (no facts) and
  // allows a bit more flexibility in constructing objects within which
  // ManifestDatalogFacts are embedded.
  ManifestDatalogFacts() {}

  ManifestDatalogFacts(std::vector<Particle> particle_instances,
                       std::vector<raksha::ir::TagClaim> claims,
                       std::vector<raksha::ir::TagCheck> checks,
                       std::vector<raksha::ir::Edge> edges)
      : particle_instances_(std::move(particle_instances)),
        claims_(std::move(claims)),
        checks_(std::move(checks)),
        edges_(std::move(edges)) {}

  // Print out all contained facts as a single datalog string. Note: this
  // does not contain the header files that would be necessary to run this
  // against the datalog scripts; it contains only facts and comments.
  std::string ToDatalog(raksha::ir::DatalogPrintContext &ctxt,
                        std::string separator = "\n") const {
    auto todatalog_formatter = [&](std::string *out, const auto &arg) {
      out->append(arg.ToDatalog(ctxt));
    };
    std::string claims_string;
    std::string checks_string;
    std::string edges_string;
    for (const auto &particle : particle_instances_) {
      ctxt.set_instantiation_map(&particle.instantiation_map());
      AppendElements(&claims_string, particle.spec()->tag_claims(), separator,
                     todatalog_formatter);
      AppendElements(&checks_string, particle.spec()->checks(), separator,
                     todatalog_formatter);
      AppendElements(&edges_string, particle.edges(), separator,
                     todatalog_formatter);
      AppendElements(&edges_string, particle.spec()->edges(), separator,
                     todatalog_formatter);
    }

    // Assemble the output and return the result.
    std::string result;
    absl::StrAppend(&result, absl::StrFormat("// Claims:%s", separator));
    absl::StrAppend(&result, claims_string);
    absl::StrAppend(&result, separator);
    absl::StrAppend(&result, absl::StrFormat("// Checks:%s", separator));
    absl::StrAppend(&result, checks_string);
    absl::StrAppend(&result, separator);
    absl::StrAppend(&result, absl::StrFormat("// Edges:%s", separator));
    absl::StrAppend(&result, edges_string);
    absl::StrAppend(&result, separator);
    return result;
  }

  const std::vector<raksha::ir::TagClaim> &claims() const { return claims_; }

  const std::vector<raksha::ir::TagCheck> &checks() const { return checks_; }

  const std::vector<raksha::ir::Edge> &edges() const { return edges_; }

 private:
  // Appends the list of elements as newline separated strings to target.
  template <typename T, typename U>
  static void AppendElements(std::string *target, const T &elements,
                             const std::string& separator,
                             const U &formatter) {
    absl::StrAppend(target, absl::StrJoin(elements, separator, formatter));
    if (!elements.empty()) {
      absl::StrAppend(target, separator);
    }
  }

  std::vector<Particle> particle_instances_;
  std::vector<raksha::ir::TagClaim> claims_;
  std::vector<raksha::ir::TagCheck> checks_;
  std::vector<raksha::ir::Edge> edges_;
};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_
