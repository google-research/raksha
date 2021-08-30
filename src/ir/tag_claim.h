#ifndef SRC_IR_TAG_CLAIM_H_
#define SRC_IR_TAG_CLAIM_H_

#include "absl/strings/str_cat.h"
#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "src/ir/tag_annotation_on_access_path.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

class TagClaim {
 public:
  // Create a tag claim from an arcs Assume proto. Despite the fact
  // that the access path indicated by a claim indicates a root, it's not
  // rooted deeply enough for us; the root goes only to the particle spec name,
  // whereas we'd like to go all the way to the recipe name. Therefore, the
  // resulting TagClaim has a SpecAccessPathRoot.
  static TagClaim CreateFromProto(
      const arcs::ClaimProto_Assume &assume_proto) {
    return TagClaim(
        TagAnnotationOnAccessPath::CreateFromProto(assume_proto, "Assume"));
  }

  // Return a TagClaim that is the same as *this, but with the root new_root.
  // Note that this expects the current root to be uninstantiated.
  TagClaim Instantiate(AccessPathRoot new_root) const {
    return TagClaim(
        tag_annotation_on_access_path_.Instantiate(std::move(new_root)));
  }

  // Produce a string containing a datalog fact for this TagClaim.
  std::string ToString() const {
    return absl::StrCat(
        "claimHasTag(\"",
        tag_annotation_on_access_path_.access_path().ToString(),
        "\", \"", tag_annotation_on_access_path_.tag(), "\").\n");
  }

 private:
  explicit TagClaim(TagAnnotationOnAccessPath tag_annotation_on_access_path)
  : tag_annotation_on_access_path_(std::move(tag_annotation_on_access_path)) {}

   // Internally, we represent the data as a TagAnnotationOnAccessPath object.
  // This allows us to share code with TagCheck. While TagCheck and TagClaim
  // are semantically very different, they are very similar in their internal
  // representation, both here and in Arcs manifest protos.
  TagAnnotationOnAccessPath tag_annotation_on_access_path_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
