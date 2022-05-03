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

#ifndef SRC_FRONTENDS_ARCS_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_
#define SRC_FRONTENDS_ARCS_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_

#include <vector>

#include "absl/strings/str_format.h"
#include "src/frontends/arcs//particle_spec.h"
#include "src/frontends/arcs/datalog_print_context.h"
#include "src/frontends/arcs/edge.h"
#include "src/frontends/arcs/system_spec.h"
#include "src/frontends/arcs/tag_check.h"
#include "src/frontends/arcs/tag_claim.h"
#include "src/ir/types/type_factory.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::xform_to_datalog {

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
    Particle(const frontends::arcs::ParticleSpec *spec,
             frontends::arcs::DatalogPrintContext::AccessPathInstantiationMap
                 &&instantiation_map,
             std::vector<frontends::arcs::Edge> &&edges)
        : spec_(spec), instantiation_map_(instantiation_map), edges_(edges) {}

    const frontends::arcs::ParticleSpec *spec() const { return spec_; }
    const frontends::arcs::DatalogPrintContext::AccessPathInstantiationMap &
    instantiation_map() const {
      return instantiation_map_;
    }
    const std::vector<frontends::arcs::Edge> &edges() const { return edges_; }

   private:
    const frontends::arcs::ParticleSpec *spec_;
    frontends::arcs::DatalogPrintContext::AccessPathInstantiationMap
        instantiation_map_;
    std::vector<frontends::arcs::Edge> edges_;
  };

  static ManifestDatalogFacts CreateFromManifestProto(
      ir::types::TypeFactory &type_factory,
      const frontends::arcs::SystemSpec &system_spec,
      const ::arcs::ManifestProto &manifest_proto);

  // A default constructor creates a sensible, legal state (no facts) and
  // allows a bit more flexibility in constructing objects within which
  // ManifestDatalogFacts are embedded.
  ManifestDatalogFacts() {}

  ManifestDatalogFacts(std::vector<Particle> particle_instances)
      : particle_instances_(std::move(particle_instances)) {}

  // Print out all contained facts as a single datalog string. Note: this
  // does not contain the header files that would be necessary to run this
  // against the datalog scripts; it contains only facts and comments.
  std::string ToDatalog(raksha::frontends::arcs::DatalogPrintContext &ctxt,
                        std::string separator = "\n") const {
    std::string claims;
    std::string checks;
    std::string edges;
    for (const auto &particle : particle_instances_) {
      ctxt.set_instantiation_map(&particle.instantiation_map());
      AppendElements(&claims, ctxt, particle.spec()->tag_claims(), separator);
      AppendElements(&checks, ctxt, particle.spec()->checks(), separator);
      AppendElements(&edges, ctxt, particle.edges(), separator);
      AppendElements(&edges, ctxt, particle.spec()->edges(), separator);
    }

    // Assemble the output and return the result.
    std::string result;
    absl::StrAppend(&result, absl::StrFormat("// Claims:%s", separator));
    absl::StrAppend(&result, claims);
    absl::StrAppend(&result, separator);
    absl::StrAppend(&result, absl::StrFormat("// Checks:%s", separator));
    absl::StrAppend(&result, checks);
    absl::StrAppend(&result, separator);
    absl::StrAppend(&result, absl::StrFormat("// Edges:%s", separator));
    absl::StrAppend(&result, edges);
    absl::StrAppend(&result, separator);
    return result;
  }

 private:
  // Appends the list of elements as newline separated strings to target.
  template <typename T>
  static void AppendElements(std::string *target,
                             raksha::frontends::arcs::DatalogPrintContext &ctxt,
                             const T &elements, const std::string &separator) {
    auto formatter = [&ctxt](std::string *out, const auto &arg) {
      out->append(arg.ToDatalog(ctxt));
    };
    absl::StrAppend(target, absl::StrJoin(elements, separator, formatter));
    if (!elements.empty()) {
      absl::StrAppend(target, separator);
    }
  }

  std::vector<Particle> particle_instances_;
};

}  // namespace raksha::frontends::arcs::xform_to_datalog

#endif  // SRC_FRONTENDS_ARCS_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_H_
