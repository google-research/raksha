#ifndef SRC_IR_TAG_CLAIM_H_
#define SRC_IR_TAG_CLAIM_H_

#include "absl/strings/str_cat.h"
#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

// This is a TagClaim that is "unrooted"; it is a claim upon a particular
// SelectorAccessPath, but does not yet have full root information. This will
// be the case for claims in ParticleSpecs until we visit the Particle(s)
// that implement that ParticleSpec. This class can have a root provided to
// it on the fly as a parameter to the ToStringWithRoot function, or can be
// associated with a root via the RootInstantiated class.
class UnrootedTagClaim {
 public:
  // Create an unrooted tag claim from an arcs Assume proto. Despite the fact
  // that the access path indicated by a claim indicates a root, it's not
  // rooted deeply enough for us; the root goes only to the particle spec name,
  // whereas we'd like to go all the way to the recipe name.
  static UnrootedTagClaim CreateFromProto(
      const arcs::ClaimProto_Assume &assume_proto);

  explicit UnrootedTagClaim(
      AccessPathSelectors access_path_selectors, std::string tag)
  : access_path_selectors_(std::move(access_path_selectors)),
    tag_(std::move(tag)) {}

  // Print out a tag claim for this class with the provided root. While this
  // can be called by anyone, it is likely that this will be called through
  // a RootInstantiated wrapper's ToString function.
  std::string ToStringWithRoot(std::string root) const {
    return absl::StrCat(
        "claimHasTag(\"",
        AccessPath(std::move(root), access_path_selectors_).ToString(),
        "\", \"", tag_, "\").\n");
  }

 private:
  // The AccessPathSelectors which this object claims has the tag tag_.
  AccessPathSelectors access_path_selectors_;
  // The tag claimed to be on access_path_selectors_ by this object.
  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
