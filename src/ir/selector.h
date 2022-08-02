//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
#ifndef SRC_FRONTENDS_ARCS_SELECTOR_H_
#define SRC_FRONTENDS_ARCS_SELECTOR_H_

#include <string>
#include <variant>

#include "src/common/logging/logging.h"
#include "src/ir/field_selector.h"

namespace raksha::ir {

// An individual selector in an AccessPath descending down the type tree.
// This Selector is generic, and may contain any method of descending, such
// as field access, array access, tuple element access, etc.
//
// For now, however, we only have implemented the FieldSelector portion.
class Selector {
 public:
  explicit Selector(FieldSelector field_selector)
      : specific_selector_(std::move(field_selector)) {}

  std::variant<FieldSelector> variant() const { return specific_selector_; }

  // Print the string representing a selector's participation in the
  // AccessPath. This will include the punctuation indicating the method of
  // selection (such as . for string, [ for index, etc) and the string
  // contents of the selector itself.
  std::string ToString() const {
    return std::visit(
        [](const auto &specific_selector) {
          return specific_selector.ToString();
        },
        specific_selector_);
  }

  // Whether two selectors are equal. Will be true if they are the same type
  // of selector and those two types also compare equal.
  //
  // std::variant has a pre-packaged operator equals that handles the dispatch
  // to the specific selector as we would expect.
  bool operator==(const Selector &other) const {
    return specific_selector_ == other.specific_selector_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const Selector &selector) {
    return H::combine(std::move(h), selector.specific_selector_);
  }

 private:
  // The variant storing the specific type of selector. We will dispatch
  // through this to perform all methods on the generic Selector.
  std::variant<FieldSelector> specific_selector_;
};

}  // namespace raksha::ir

#endif  // SRC_FRONTENDS_ARCS_SELECTOR_H_
