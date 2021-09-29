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
//----------------------------------------------------------------------------

#ifndef SRC_IR_TAG_CHECK_H_
#define SRC_IR_TAG_CHECK_H_

#include "absl/strings/str_format.h"
#include "src/ir/tag_annotation_on_access_path.h"
#include "src/ir/instantiator.h"
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

  // Print out the tag check as a Datalog fact. This is currently very
  // simplified relative to the arbitrary boolean expression predicate we
  // will eventually want, but is enough to get some simple Arcs tests
  // passing for the MVP.
  std::string ToDatalog(const Instantiator &instantiator) const {
    constexpr absl::string_view kCheckHasTagFormat =
        R"(checkHasTag("%s", "%s") :- mayHaveTag("%s", "%s").)";
    std::string access_path =
        tag_annotation_on_access_path_.access_path().ToString(instantiator);
    std::string tag = tag_annotation_on_access_path_.tag();

    return absl::StrFormat(kCheckHasTagFormat, access_path, tag, access_path,
                           tag);
  }

  bool operator==(const TagCheck &other) const {
    return tag_annotation_on_access_path_ ==
      other.tag_annotation_on_access_path_;
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
