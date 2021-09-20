#ifndef SRC_IR_TAG_CLAIM_H_
#define SRC_IR_TAG_CLAIM_H_

#include "absl/strings/str_format.h"
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
      std::string claiming_particle_name,
      const arcs::ClaimProto_Assume &assume_proto) {
    return TagClaim(
        std::move(claiming_particle_name),
        TagAnnotationOnAccessPath::CreateFromProto(assume_proto, "Assume"));
  }

  explicit TagClaim(
      std::string claiming_particle_name,
      TagAnnotationOnAccessPath tag_annotation_on_access_path)
    : claiming_particle_name_(std::move(claiming_particle_name)),
      tag_annotation_on_access_path_(std::move(tag_annotation_on_access_path))
    {}

  // Return a TagClaim that is the same as *this, but with the root new_root.
  // Note that this expects the current root to be uninstantiated.
  TagClaim Instantiate(AccessPathRoot new_root) const {
    return TagClaim(
        claiming_particle_name_,
        tag_annotation_on_access_path_.Instantiate(std::move(new_root)));
  }

  // Allow this TagClaim to participate in a bulk instantiation of multiple
  // uninstantiated AccessPaths.
  TagClaim BulkInstantiate(
      const absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
          &instantiation_map) const {
    return TagClaim(
        claiming_particle_name_,
        tag_annotation_on_access_path_.BulkInstantiate(instantiation_map));
  }

  // Produce a string containing a datalog fact for this TagClaim.
  std::string ToDatalog() const {
    constexpr absl::string_view kClaimHasTagFormat =
        R"(claimHasTag("%s", "%s", "%s").)";
    return absl::StrFormat(
        kClaimHasTagFormat,
        claiming_particle_name_,
        tag_annotation_on_access_path_.access_path().ToString(),
        tag_annotation_on_access_path_.tag());
  }

  bool operator==(const TagClaim &other) const {
    return
      (claiming_particle_name_ == other.claiming_particle_name_) &&
      (tag_annotation_on_access_path_ == other.tag_annotation_on_access_path_);
  }

 private:
  // The name of the particle performing this claim. Important for connecting
  // the claim to a principal for authorization logic purposes.
  std::string claiming_particle_name_;
  // Internally, we represent the data as a TagAnnotationOnAccessPath object.
  // This allows us to share code with TagCheck. While TagCheck and TagClaim
  // are semantically very different, they are very similar in their internal
  // representation, both here and in Arcs manifest protos.
  TagAnnotationOnAccessPath tag_annotation_on_access_path_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
