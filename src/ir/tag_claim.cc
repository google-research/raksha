#include "src/ir/tag_claim.h"

#include "src/ir/access_path_root.h"

namespace raksha::ir {

TagClaim TagClaim::CreateFromProto(
    const arcs::ClaimProto_Assume &assume_proto) {
  CHECK(assume_proto.has_access_path())
    << "Expected Assume claim message to have access_path field.";
  const arcs::AccessPathProto &access_path_proto = assume_proto.access_path();
  AccessPathSelectors access_path_selectors =
      AccessPathSelectors::CreateFromProto(access_path_proto);
  // The proto contains an access path only from the particle spec through
  // the type. We need to root it at the recipe. Therefore, fill the
  // AccessPath's root with a SpecAccessPathRoot for now.
  AccessPath access_path =
      AccessPath::CreateSpecAccessPath(std::move(access_path_selectors));
  CHECK(assume_proto.has_predicate())
    << "Expected Assume claim message to have predicate field.";
  const arcs::InformationFlowLabelProto_Predicate &predicate =
      assume_proto.predicate();
  CHECK(predicate.has_label())
    << "Assume predicates other than simple tags are not yet supported.";
  const arcs::InformationFlowLabelProto &label = predicate.label();
  CHECK(label.has_semantic_tag())
    << "semantic_tag field required on InformationFlowLabelProto.";
  return TagClaim(std::move(access_path), label.semantic_tag());
}

}  // namespace raksha::ir
