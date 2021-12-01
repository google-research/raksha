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

#include "src/ir/proto/predicate.h"

#include <memory>

#include "src/ir/predicate.h"

namespace raksha::ir::proto {

std::unique_ptr<Predicate> Decode(
    const arcs::InformationFlowLabelProto_Predicate &predicate_proto) {
  switch (predicate_proto.predicate_case()) {
    case arcs::InformationFlowLabelProto_Predicate::kLabel: {
      const arcs::InformationFlowLabelProto &label = predicate_proto.label();
      CHECK(label.has_semantic_tag())
        << "Found a label without required field tag.";
      return std::make_unique<TagPresence>(label.semantic_tag());
    }
    case arcs::InformationFlowLabelProto_Predicate::kAnd: {
      const arcs::InformationFlowLabelProto_Predicate_And &and_predicate =
          predicate_proto.and_();
      CHECK(and_predicate.has_conjunct0())
        << "Found an `And` predicate without required field conjunct0.";
      CHECK(and_predicate.has_conjunct1())
        << "Found an `And` predicate without required field conjunct1.";
      return std::make_unique<And>(Decode(and_predicate.conjunct0()),
                                   Decode(and_predicate.conjunct1()));
    }
    case arcs::InformationFlowLabelProto_Predicate::kImplies: {
       const arcs::InformationFlowLabelProto_Predicate_Implies
        &implies_predicate = predicate_proto.implies();
       CHECK(implies_predicate.has_antecedent())
        << "Found an `Implies` predicate without required field antecedent.";
       CHECK(implies_predicate.has_consequent())
        << "Found an `Implies` predicate without required field consequent.";

       return std::make_unique<Implies>(Decode(implies_predicate.antecedent()),
                                        Decode(implies_predicate.consequent()));
    }
    case arcs::InformationFlowLabelProto_Predicate::kNot: {
      const arcs::InformationFlowLabelProto_Predicate_Not &not_predicate =
          predicate_proto.not_();
      CHECK(not_predicate.has_predicate())
        << "Found a `Not` predicate without required field predicate.";
      return std::make_unique<Not>(Decode(not_predicate.predicate()));
    }
    case arcs::InformationFlowLabelProto_Predicate::kOr: {
      const arcs::InformationFlowLabelProto_Predicate_Or &or_predicate =
          predicate_proto.or_();
      CHECK(or_predicate.has_disjunct0())
        << "Found an `Or` predicate without required field disjunct0.";
      CHECK(or_predicate.has_disjunct1())
        << "Found an `Or` predicate without required field disjunct1.";
      return std::make_unique<Or>(Decode(or_predicate.disjunct0()),
                                  Decode(or_predicate.disjunct1()));
    }
    default: {
      LOG(FATAL) << "Unexpected predicate kind.";
    }
  }
}

}  // namespace raksha::ir::proto
