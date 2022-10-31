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
#ifndef SRC_FRONTENDS_ARCS_ACCESS_PATH_H_
#define SRC_FRONTENDS_ARCS_ACCESS_PATH_H_

#include "absl/strings/str_cat.h"
#include "src/frontends/arcs/access_path_root.h"
#include "src/ir/access_path_selectors.h"

namespace raksha::frontends::arcs {

// An AccessPath is a fully-qualified path from the root of the data. At this
// time, it is an AccessPathSelectors, describing the path through the type
// tree, and an AccessPathRoot, describing the path before that.
class AccessPath {
 public:
  explicit AccessPath(AccessPathRoot root,
                      ir::AccessPathSelectors access_path_selectors)
      : root_(std::move(root)),
        access_path_selectors_(std::move(access_path_selectors)) {}

  // Express the AccessPath as a string. This is accomplished by
  // concatenating the root string and the selectors string.
  std::string ToString() const {
    return absl::StrCat(root_.ToString(), access_path_selectors_.ToString());
  }

  // Express the AccessPath as a datalog string. This is accomplished by
  // concatenating the root string and the selectors string.
  std::string ToDatalog(const DatalogPrintContext &ctxt) const {
    return absl::StrCat(root_.ToDatalog(ctxt),
                        access_path_selectors_.ToString());
  }

  const AccessPathRoot &root() const { return root_; }

  const ir::AccessPathSelectors &selectors() const {
    return access_path_selectors_;
  }

  bool operator==(const AccessPath &other) const {
    return (root_ == other.root_) &&
           (access_path_selectors_ == other.access_path_selectors_);
  }

  template <typename H>
  friend H AbslHashValue(H h, const AccessPath &access_path) {
    return H::combine(std::move(h), access_path.root_,
                      access_path.access_path_selectors_);
  }

 private:
  AccessPathRoot root_;
  ir::AccessPathSelectors access_path_selectors_;
};

}  // namespace raksha::frontends::arcs

#endif  // SRC_FRONTENDS_ARCS_ACCESS_PATH_H_
