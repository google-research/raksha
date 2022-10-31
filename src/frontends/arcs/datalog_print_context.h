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

#ifndef SRC_FRONTENDS_ARCS_DATALOG_PRINT_CONTEXT_H_
#define SRC_FRONTENDS_ARCS_DATALOG_PRINT_CONTEXT_H_

#include "absl/strings/str_cat.h"
#include "src/common/containers/hash_map.h"
#include "src/frontends/arcs/access_path_root.h"

namespace raksha::frontends::arcs {

// A class for providing services that require mutable state to be remembered
// during the Datalog printing process. Currently, is just used for printing
// unique labels.
class DatalogPrintContext {
 public:
  using AccessPathInstantiationMap =
      common::containers::HashMap<AccessPathRoot, AccessPathRoot>;

  DatalogPrintContext() : check_counter_(0), instantiation_map_(nullptr) {}

  // DatalogPrintContext is not copyable, as we need a single copy to be the
  // source of truth on creating unique labels. It is, however, movable.
  DatalogPrintContext(const DatalogPrintContext &) = delete;
  DatalogPrintContext &operator=(const DatalogPrintContext &) = delete;

  // Print a unique label for use in a check.
  std::string GetUniqueCheckLabel() {
    return absl::StrCat("check_num_", check_counter_++);
  }

  void set_instantiation_map(
      const AccessPathInstantiationMap *instantiation_map) {
    instantiation_map_ = instantiation_map;
  }

  const AccessPathInstantiationMap *instantiation_map() const {
    return instantiation_map_;
  }

 private:
  uint64_t check_counter_;
  const AccessPathInstantiationMap *instantiation_map_;
};

}  // namespace raksha::frontends::arcs

#endif  // SRC_FRONTENDS_ARCS_DATALOG_PRINT_CONTEXT_H_
