#ifndef SRC_IR_TAG_CLAIM_H_
#define SRC_IR_TAG_CLAIM_H_

#include "absl/strings/str_cat.h"
#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
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
      const arcs::ClaimProto_Assume &assume_proto);

  // Return a TagClaim that is the same as *this, but with the root new_root.
  // Note that this expects the current root to be uninstantiated.
  TagClaim Instantiate(AccessPathRoot new_root) const {
    return TagClaim(
        access_path_.Instantiate(std::move(new_root)), std::move(tag_));
  }

  // Produce a string containing a datalog fact for this TagClaim.
  std::string ToString() const {
    return absl::StrCat(
        "claimHasTag(\"", access_path_.ToString(), "\", \"", tag_, "\").\n");
  }

 private:
  explicit TagClaim(AccessPath access_path, std::string tag)
  : access_path_(std::move(access_path)), tag_(std::move(tag)) {}

  // The AccessPath which this object claims has the tag tag_.
  AccessPath access_path_;
  // The tag claimed to be on access_path_selectors_ by this object.
  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
