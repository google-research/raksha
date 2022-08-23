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
#ifndef SRC_FRONTENDS_ARCS_FIELD_SELECTOR_H_
#define SRC_FRONTENDS_ARCS_FIELD_SELECTOR_H_

#include <string>

#include "absl/strings/str_cat.h"

namespace raksha::ir {

// Represents descending through the type tree by taking the field of an
// EntityType in an AccessPath.
class FieldSelector {
 public:
  FieldSelector(std::string field_name) : field_name_(std::move(field_name)) {}

  const std::string &field_name() const { return field_name_; }

  // Prints the string representing dereferencing the field in the AccessPath.
  // This will just be the "." punctuation plus the name of the field.
  std::string ToString() const { return absl::StrCat(".", field_name_); }

  // Two fields selectors are equal exactly when their names are equal.
  bool operator==(const FieldSelector &other) const {
    return field_name_ == other.field_name_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const FieldSelector &field_selector) {
    return H::combine(std::move(h), field_selector.field_name_);
  }

 private:
  // All of the specialness of a FieldSelector is contained within its name.
  std::string field_name_;
};

}  // namespace raksha::ir

#endif  // SRC_FRONTENDS_ARCS_FIELD_SELECTOR_H_
