#include "src/ir/tag_claim.h"

#include "src/ir/access_path_root.h"

namespace raksha::ir {

TagClaim TagClaim::CreateFromProto(
    const arcs::ClaimProto_Assume &assume_proto) {
  CHECK(assume_proto.has_access_path())
    << "Expected Assume claim message to have access_path field.";
  const arcs::AccessPathProto &access_path_proto = assume_proto.access_path();
  AccessPath access_path = AccessPath::CreateFromProto(access_path_proto);
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
