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

#ifndef SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
#define SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_

#include <vector>

#include "absl/strings/str_format.h"
#include "src/ir/edge.h"
#include "src/ir/tag_check.h"
#include "src/ir/tag_claim.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog {

class DatalogFacts {
 public:
  DatalogFacts(ManifestDatalogFacts manifest_datalog_facts)
      : manifest_datalog_facts_(std::move(manifest_datalog_facts)) {}

  // Print out all contained facts as a single datalog string. Note: this
  // does not contain the header files that would be necessary to run this
  // against the datalog scripts; it contains only facts and comments.
  std::string ToString() const {
    return absl::StrCat(
        "// GENERATED FILE, DO NOT EDIT!\n\n", "#include \"taint.dl\"\n",
        manifest_datalog_facts_.ToString(), "\n.output checkHasTag\n");
  }

 private:
  ManifestDatalogFacts manifest_datalog_facts_;
};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
