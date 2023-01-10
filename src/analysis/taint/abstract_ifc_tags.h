//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
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
#ifndef SRC_ANALYSIS_TAINT_ABSTRACT_IFC_TAGS_H_
#define SRC_ANALYSIS_TAINT_ABSTRACT_IFC_TAGS_H_

#include "absl/base/attributes.h"
#include "absl/container/btree_set.h"
#include "src/analysis/taint/inference_rules.h"
#include "src/common/utils/intrusive_ptr.h"

namespace raksha::analysis::taint {

using TagIdSet = absl::flat_hash_set<TagId>;

class IfcTags;
using IfcTagsConstPtr = intrusive_ptr<const IfcTags>;
using IfcTagsPtr = intrusive_ptr<IfcTags>;

// Data structure for keeping around Information Flow Control (IFC) tags.
class IfcTags : public RefCounted<IfcTags> {
 public:
  static IfcTagsPtr Create(TagIdSet secrecy_tags, TagIdSet integrity_tags) {
    return IfcTagsPtr(
        new IfcTags(std::move(secrecy_tags), std::move(integrity_tags)));
  }

  IfcTags() = delete;

  const TagIdSet& secrecy_tags() const { return secrecy_tags_; }
  const TagIdSet& integrity_tags() const { return integrity_tags_; }

 private:
  IfcTags(TagIdSet secrecy_tags, TagIdSet integrity_tags)
      : secrecy_tags_(std::move(secrecy_tags)),
        integrity_tags_(std::move(integrity_tags)) {}

  TagIdSet secrecy_tags_;
  TagIdSet integrity_tags_;
};

// Abstract domain for IFC tags.
class AbstractIfcTags {
 public:
  AbstractIfcTags(TagIdSet secrecy_tags, TagIdSet integrity_tags);

  // Returns the bottom abstract value.
  static AbstractIfcTags Bottom();

  // Returns true if this abstract value is bottom.
  bool IsBottom() const;

  // Returns true if other is equivalent to this object.
  bool IsEquivalentTo(const AbstractIfcTags& other) const;

  // Returns the underlying secrecy tags. Fails if this is bottom.
  const TagIdSet& secrecy_tags() const;

  // Returns the underlying integrity tags. Fails if this is bottom.
  const TagIdSet& integrity_tags() const;

  // Returns true if the given integrity tag is set. Fails if this is bottom.
  bool HasIntegrity(TagId tag) const;

  // Returns true if the secrecy tags are all empty. Fails if this is bottom.
  bool HasNoSecrecy() const;

  // Returns a new instance that is constructed as follows:
  //  - integrity_tags is the intersection of the integrity tags of operands.
  //  - secrecy_tags is the union of the secrecy tags of operands.
  ABSL_MUST_USE_RESULT AbstractIfcTags Join(const AbstractIfcTags& other) const;

  // Returns a new value after setting the given integrity tag if not bottom.
  // Otherwise, returns bottom.
  ABSL_MUST_USE_RESULT AbstractIfcTags SetIntegrity(TagId tag) const;

  // Returns a new value after clearing the given integrity tag if not bottom.
  // Otherwise, returns bottom.
  ABSL_MUST_USE_RESULT AbstractIfcTags ClearIntegrity(TagId tag) const;

  // Returns a new value after setting  the given secrecy tag if not bottom.
  // Otherwise, returns bottom.
  ABSL_MUST_USE_RESULT AbstractIfcTags SetSecrecy(TagId tag) const;

  // Returns a new value after clearing the given secrecy tag if not bottom.
  // Otherwise, returns bottom.
  ABSL_MUST_USE_RESULT AbstractIfcTags ClearSecrecy(TagId tag) const;

 private:
  AbstractIfcTags(IfcTagsConstPtr tags) : tags_(std::move(tags)) {}

  IfcTagsConstPtr tags_;
};

}  // namespace raksha::analysis::taint

#endif  // SRC_ANALYSIS_TAINT_ABSTRACT_IFC_TAGS_H_
