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
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::xform_to_datalog {

// A utility class that is used to generate a datalog program by combining
// datalog facts from different sources and adding the necessary headers.
class DatalogFacts {
 public:
  DatalogFacts(ManifestDatalogFacts manifest_datalog_facts,
               AuthorizationLogicDatalogFacts auth_logic_datalog_facts)
    : manifest_datalog_facts_(std::move(manifest_datalog_facts)),
      auth_logic_datalog_facts_(std::move(auth_logic_datalog_facts))
  {}

  // Returns the datalog program with necessary headers.
  std::string ToDatalog() const {
    return absl::StrCat(
        kDatalogFilePrefix,
        manifest_datalog_facts_.ToDatalog(),
        auth_logic_datalog_facts_.ToDatalog());
  }

 private:
  ManifestDatalogFacts manifest_datalog_facts_;
  AuthorizationLogicDatalogFacts auth_logic_datalog_facts_;

  // The prefix that should be added to the datalog program.
  static constexpr char kDatalogFilePrefix[] =
      R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
.output checkHasTag
)";

};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
