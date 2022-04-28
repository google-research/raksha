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

#include "src/frontends/arcs/proto/tag_claim.h"

namespace raksha::frontends::arcs::proto {

static std::vector<TagClaim> GetTagClaimsFromPredicate(
    std::string claiming_particle_name, AccessPath access_path,
    const ::arcs::InformationFlowLabelProto_Predicate &predicate,
    const bool in_negation) {
  switch (predicate.predicate_case()) {
    // Base case: if it is a label, construct a TagClaim with the given info.
    case ::arcs::InformationFlowLabelProto_Predicate::kLabel: {
      const ::arcs::InformationFlowLabelProto &label = predicate.label();
      CHECK(label.has_semantic_tag())
          << "semantic_tag field required on InformationFlowLabelProto.";
      bool claim_tag_is_present = !in_negation;
      return std::vector<TagClaim>{
          TagClaim(std::move(claiming_particle_name), std::move(access_path),
                   claim_tag_is_present, label.semantic_tag())};
    }
    case ::arcs::InformationFlowLabelProto_Predicate::kNot: {
      CHECK(!in_negation) << "Double negation not allowed in Assume claim.";
      ::arcs::InformationFlowLabelProto_Predicate_Not not_predicate =
          predicate.not_();
      CHECK(not_predicate.has_predicate())
          << "Inner predicate is required in Not predicate in Assume.";
      return GetTagClaimsFromPredicate(
          std::move(claiming_particle_name), std::move(access_path),
          not_predicate.predicate(), true /*in_negation*/);
    }
    case ::arcs::InformationFlowLabelProto_Predicate::kAnd: {
      CHECK(!in_negation) << "Negated Ands not allowed in Assume claim.";
      ::arcs::InformationFlowLabelProto_Predicate_And and_predicate =
          predicate.and_();
      CHECK(and_predicate.has_conjunct0())
          << "And predicate missing required conjunct0 field.";
      CHECK(and_predicate.has_conjunct1())
          << "And predicate missing required conjunct1 field.";
      std::vector<TagClaim> and_result_claims = GetTagClaimsFromPredicate(
          claiming_particle_name, access_path, and_predicate.conjunct0(),
          false /*in_negation*/);
      std::vector<TagClaim> conjunct1_claims = GetTagClaimsFromPredicate(
          claiming_particle_name, access_path, and_predicate.conjunct1(),
          false /*in_negation*/);
      and_result_claims.insert(
          and_result_claims.end(),
          std::make_move_iterator(conjunct1_claims.begin()),
          std::make_move_iterator(conjunct1_claims.end()));
      return and_result_claims;
    }
    default: {
      LOG(FATAL) << "Found unexpected predicate kind in Assume claim proto.";
    }
  }
}

// Create a list of TagClaims from an Assume proto. Each of these will be
// uninstantiated and thus rooted at the ParticleSpec.
std::vector<TagClaim> Decode(std::string claiming_particle_name,
                             const ::arcs::ClaimProto_Assume &assume_proto) {
  CHECK(assume_proto.has_access_path())
      << "Expected Assume message to have access_path field.";
  const ::arcs::AccessPathProto &access_path_proto = assume_proto.access_path();
  AccessPath access_path = proto::Decode(access_path_proto);
  CHECK(assume_proto.has_predicate())
      << "Expected Assume message to have predicate field.";
  const ::arcs::InformationFlowLabelProto_Predicate &predicate =
      assume_proto.predicate();

  // Now that we have gathered the top-level information from the Assume
  // proto, descend into the predicate to construct the list of claims.
  return GetTagClaimsFromPredicate(std::move(claiming_particle_name),
                                   std::move(access_path), predicate,
                                   /*in_negation=*/false);
}

}  // namespace raksha::frontends::arcs::proto
