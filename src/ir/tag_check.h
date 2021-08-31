#ifndef SRC_IR_TAG_CHECK_H_
#define SRC_IR_TAG_CHECK_H_

#include "src/ir/tag_annotation_on_access_path.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

// A class representing a check of some predicate involving tags on a
// particular AccessPath. At the moment, this is very simplified, allowing
// for testing only the presence of a single tag on a particular AccessPath.
class TagCheck {
 public:
  static TagCheck CreateFromProto(const arcs::CheckProto &check_proto) {
    return TagCheck(
        TagAnnotationOnAccessPath::CreateFromProto(check_proto, "Check"));
  }

  TagCheck(TagAnnotationOnAccessPath tag_annotation_on_access_path)
    : tag_annotation_on_access_path_(tag_annotation_on_access_path) {}

  TagCheck Instantiate(AccessPathRoot access_path_root) const {
    return TagCheck(
        tag_annotation_on_access_path_.Instantiate(access_path_root));
  }

  // Print out the tag check as a Datalog fact. This is currently very
  // simplified relative to the arbitrary boolean expression predicate we
  // will eventually want, but is enough to get some simple Arcs tests
  // passing for the MVP.
  std::string ToString() const {
    return absl::StrCat(
        "checkHasTag(\"",
        tag_annotation_on_access_path_.access_path().ToString(),
        "\", \"",
        tag_annotation_on_access_path_.tag(),
        "\").\n");
  }

 private:
  // Internally, we represent the data as a TagAnnotationOnAccessPath object.
  // This allows us to share code with TagClaim. While TagCheck and TagClaim
  // are semantically very different, they are very similar in their internal
  // representation, both here and in Arcs manifest protos.
  TagAnnotationOnAccessPath tag_annotation_on_access_path_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CHECK_H_
