#include "src/ir/tag_claim.h"

namespace raksha::ir {

UnrootedTagClaim UnrootedTagClaim::CreateFromProto(
    const arcs::ClaimProto_Assume &assume_proto) {
  CHECK(assume_proto.has_access_path())
    << "Expected Assume claim message to have access_path field.";
  const arcs::AccessPathProto &access_path_proto = assume_proto.access_path();
  AccessPathSelectors access_path_selectors =
      AccessPathSelectors::CreateFromProto(access_path_proto);
  CHECK(assume_proto.has_predicate())
    << "Expected Assume claim message to have predicate field.";
  const arcs::InformationFlowLabelProto_Predicate &predicate =
      assume_proto.predicate();
  CHECK(predicate.has_label())
    << "Assume predicates other than simple tags are not yet supported.";
  const arcs::InformationFlowLabelProto &label = predicate.label();
  CHECK(label.has_semantic_tag())
    << "semantic_tag field required on InformationFlowLabelProto.";
  return UnrootedTagClaim(
      std::move(access_path_selectors), label.semantic_tag());
}

}  // namespace raksha::ir
