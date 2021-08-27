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
// that implement that ParticleSpec. This class is used only to eventually
// produce a full TagClaim.
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

  // Make the TagClaim class a friend. This allows us to make the
  // UnrootedTagClaim otherwise completely opaque. The TagClaim will read the
  // private fields during its constructor.
  friend class TagClaim;

 private:
  // The AccessPathSelectors which this object claims has the tag tag_.
  AccessPathSelectors access_path_selectors_;
  // The tag claimed to be on access_path_selectors_ by this object.
  std::string tag_;
};

// A claim that a particular access path has a particular tag. This is how
// tags are introduced to the dataflow graph.
class TagClaim {
 public:
  explicit TagClaim(
      std::string root_string, UnrootedTagClaim unrooted_tag_claim)
      : access_path_(
          std::move(root_string),
          std::move(unrooted_tag_claim.access_path_selectors_)),
        tag_(std::move(unrooted_tag_claim.tag_)) {}

  // Produce a string from the TagClaim containing an equivalent datalog fact.
  std::string ToString() const {
    return absl::StrCat(
        "claimHasTag(\"", access_path_.ToString(), "\", \"", tag_, "\").\n");
  }

 private:
  // The full, rooted access path that this object claims has the tag tag_.
  AccessPath access_path_;
  // The tag claimed to be on access_path_ by this object.
  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
