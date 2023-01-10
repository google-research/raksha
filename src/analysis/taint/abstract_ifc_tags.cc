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
#include "src/analysis/taint/abstract_ifc_tags.h"

#include "absl/algorithm/container.h"
#include "src/common/utils/ref_counted.h"

namespace raksha::analysis::taint {

AbstractIfcTags::AbstractIfcTags(TagIdSet secrecy_tags, TagIdSet integrity_tags)
    : tags_(IfcTags::Create(std::move(secrecy_tags),
                            std::move(integrity_tags))) {}

/*static*/ AbstractIfcTags AbstractIfcTags::Bottom() {
  return AbstractIfcTags(nullptr);
}

bool AbstractIfcTags::IsBottom() const { return tags_ == nullptr; }

bool AbstractIfcTags::IsEquivalentTo(const AbstractIfcTags& other) const {
  if (this->IsBottom()) return other.IsBottom();
  if (other.IsBottom()) return false;
  return tags_->integrity_tags() == other.tags_->integrity_tags() &&
         tags_->secrecy_tags() == other.tags_->secrecy_tags();
}

AbstractIfcTags AbstractIfcTags::Join(const AbstractIfcTags& other) const {
  if (this->IsBottom()) return other;
  if (other.IsBottom()) return *this;

  // Compute union of secrecy tags
  TagIdSet new_secrecy_tags = tags_->secrecy_tags();
  absl::c_copy(other.tags_->secrecy_tags(),
               std::inserter(new_secrecy_tags, new_secrecy_tags.end()));

  // Compute interserction of integrity tags.
  TagIdSet new_integrity_tags;
  const TagIdSet& other_integrity_tags = other.tags_->integrity_tags();
  for (const TagId& tag : tags_->integrity_tags()) {
    if (other_integrity_tags.contains(tag)) new_integrity_tags.insert(tag);
  }
  return AbstractIfcTags(std::move(new_secrecy_tags),
                         std::move(new_integrity_tags));
}

AbstractIfcTags AbstractIfcTags::SetIntegrity(TagId tag) const {
  if (IsBottom()) return *this;

  TagIdSet new_integrity_tags = tags_->integrity_tags();
  new_integrity_tags.insert(tag);
  return AbstractIfcTags(tags_->secrecy_tags(), std::move(new_integrity_tags));
}

AbstractIfcTags AbstractIfcTags::ClearIntegrity(TagId tag) const {
  if (IsBottom()) return *this;

  TagIdSet new_integrity_tags = tags_->integrity_tags();
  new_integrity_tags.erase(tag);
  return AbstractIfcTags(tags_->secrecy_tags(), std::move(new_integrity_tags));
}

AbstractIfcTags AbstractIfcTags::SetSecrecy(TagId tag) const {
  if (IsBottom()) return *this;

  TagIdSet new_secrecy_tags = tags_->secrecy_tags();
  new_secrecy_tags.insert(tag);
  return AbstractIfcTags(std::move(new_secrecy_tags), tags_->integrity_tags());
}

AbstractIfcTags AbstractIfcTags::ClearSecrecy(TagId tag) const {
  if (IsBottom()) return *this;

  TagIdSet new_secrecy_tags = tags_->secrecy_tags();
  new_secrecy_tags.erase(tag);
  return AbstractIfcTags(std::move(new_secrecy_tags), tags_->integrity_tags());
}

const TagIdSet& AbstractIfcTags::secrecy_tags() const {
  CHECK(!IsBottom()) << "Cannot return secrecy tags for bottom.";
  return tags_->secrecy_tags();
}

const TagIdSet& AbstractIfcTags::integrity_tags() const {
  CHECK(!IsBottom()) << "Cannot return integrity tags for bottom.";
  return tags_->integrity_tags();
}

bool AbstractIfcTags::HasIntegrity(TagId tag) const {
  CHECK(!IsBottom()) << "Cannot check integrity for bottom.";
  return tags_->integrity_tags().contains(tag);
}

bool AbstractIfcTags::HasNoSecrecy() const {
  CHECK(!IsBottom()) << "Cannot check secrecy for bottom.";
  return tags_->secrecy_tags().empty();
}

}  // namespace raksha::analysis::taint
