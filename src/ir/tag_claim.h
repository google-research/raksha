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

#ifndef SRC_IR_TAG_CLAIM_H_
#define SRC_IR_TAG_CLAIM_H_

#include "absl/strings/str_format.h"
#include "src/common/logging/logging.h"
#include "src/ir/access_path.h"
#include "src/ir/datalog_print_context.h"
#include "src/ir/proto/access_path.h"

namespace raksha::ir {

// Represents a claim that adds or removes a single tag from a particular
// access path.
class TagClaim {
 public:
  explicit TagClaim(std::string claiming_particle_name, AccessPath access_path,
                    bool claim_tag_is_present, std::string tag)
      : claiming_particle_name_(std::move(claiming_particle_name)),
        access_path_(std::move(access_path)),
        claim_tag_is_present_(claim_tag_is_present),
        tag_(std::move(tag)) {}

  // Produce a string containing a datalog fact for this TagClaim.
  std::string ToDatalog(DatalogPrintContext &ctxt) const {
    constexpr absl::string_view kClaimTagFormat =
        R"(%s("%s", "%s", owner, "%s") :- ownsAccessPath(owner, "%s").)";
    absl::string_view relation_name =
        (claim_tag_is_present_) ? "says_hasTag" : "says_removeTag";
    return absl::StrFormat(
        kClaimTagFormat, relation_name, claiming_particle_name_,
        access_path_.ToDatalog(ctxt), tag_, access_path_.ToDatalog(ctxt));
  }

  bool operator==(const TagClaim &other) const {
    return (claiming_particle_name_ == other.claiming_particle_name_) &&
           (access_path_ == other.access_path_) &&
           (claim_tag_is_present_ == other.claim_tag_is_present_) &&
           (tag_ == other.tag_);
  }

  const AccessPath &access_path() const { return access_path_; }
  const std::string &tag() const { return tag_; }
  bool claim_tag_is_present() const { return claim_tag_is_present_; }

 private:
  // The name of the particle performing this claim. Important for connecting
  // the claim to a principal for authorization logic purposes.
  std::string claiming_particle_name_;
  // The access path upon which the claim is being made.
  AccessPath access_path_;
  // If true, we are claiming that the tag is present. If false, we are
  // claiming that the tag is absent.
  bool claim_tag_is_present_;
  // The tag being claimed.
  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_TAG_CLAIM_H_
