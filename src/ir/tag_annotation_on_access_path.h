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
//-----------------------------------------------------------------------------

#ifndef SRC_IR_TAG_ANNOTATION_ON_ACCESS_PATH_SELECTORS_H_
#define SRC_IR_TAG_ANNOTATION_ON_ACCESS_PATH_SELECTORS_H_

#include <string>

#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/proto/access_path.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

// This represents a predicate that describes a tag annotated on a particular
// AccessPath. Both claims and checks of tags, despite having different usages,
// actually have a very similar internal structure: they designate some
// AccessPath and indicate a tag (or, in the future, an arbitrary boolean
// predicate involving tags) upon it. The only difference is that a claim
// introduces or removes a tag upon that path while the check reads state to
// ensure that the tag is present or not. In addition, the two have very
// similar internal representation in the Arcs manifest protos, allowing us
// to use the same code to create the two different structures.
//
// TODO(#138): The above comment is actually no longer fully true. While it
//  remains the case that the protos that represent tag claims and checks
//  have similar internal structure, that structure is parsed very
//  differently, as claims allow only "and" and "not" predicates, while
//  checks can check arbitrary boolean expressions on the presence of tags.
//  Claim is already no longer relying upon this class; it should be
//  eliminated when we fix #137.
class TagAnnotationOnAccessPath {
 public:
  template<class T>
  static TagAnnotationOnAccessPath CreateFromProto(
      const T &proto, const std::string &proto_name) {
    CHECK(proto.has_access_path())
      << "Expected " << proto_name << " message to have access_path field.";
    const arcs::AccessPathProto &access_path_proto = proto.access_path();
    AccessPath access_path = proto::Decode(access_path_proto);
    CHECK(proto.has_predicate())
      << "Expected " << proto_name << " message to have predicate field.";
    const arcs::InformationFlowLabelProto_Predicate &predicate =
        proto.predicate();
    CHECK(predicate.has_label())
      << proto_name
      << " predicates other than simple tags are not yet supported.";
    const arcs::InformationFlowLabelProto &label = predicate.label();
    CHECK(label.has_semantic_tag())
      << "semantic_tag field required on InformationFlowLabelProto.";
    return TagAnnotationOnAccessPath(access_path, label.semantic_tag());
  }

  explicit TagAnnotationOnAccessPath(
      AccessPath access_path, std::string tag)
      : access_path_(access_path), tag_(tag) {}

  const AccessPath &access_path() const {
    return access_path_;
  }

  const std::string &tag() const {
    return tag_;
  }

  bool operator==(const TagAnnotationOnAccessPath &other) const {
    return (access_path_ == other.access_path_) && (tag_ == other.tag_);
  }

 private:
  // The AccessPath upon which the tag is annotated.
  AccessPath access_path_;
  // The tag being annotated on the AccessPath.
  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_ANNOTATION_ON_ACCESS_PATH_SELECTORS_H_
