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
#include "src/ir/datalog_print_context.h"
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
  std::string ToDatalog(raksha::ir::DatalogPrintContext &ctxt) const {
    return absl::StrFormat(
        kDatalogFileFormat,
        manifest_datalog_facts_.ToDatalog(ctxt),
        auth_logic_datalog_facts_.ToDatalog());
  }

 private:
  ManifestDatalogFacts manifest_datalog_facts_;
  AuthorizationLogicDatalogFacts auth_logic_datalog_facts_;

  // The prefix that should be added to the datalog program.
  static constexpr char kDatalogFileFormat[] =
      R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
#include "may_will.dl"

// Rules for detecting policy failures.
.decl testFails(check_index: symbol)
.output testFails(IO=stdout)
.decl allTests(check_index: symbol)
.output allTests(IO=stdout)
.decl duplicateTestCaseNames(testAspectName: symbol)
.output duplicateTestCaseNames(IO=stdout)

.output disallowedUsage(IO=stdout)

.decl isCheck(check_index: symbol)
.decl check(check_index: symbol)

allTests(check_index) :- isCheck(check_index).
testFails(check_index) :-
  isCheck(check_index), !check(check_index).

testFails("may_will") :- disallowedUsage(_, _, _). 

// Rules for linking generated relations with the ones in the dl program.
.decl says_ownsTag(speaker: Principal, owner: Principal, tag: Tag)
saysOwnsTag(x, y, z) :- says_ownsTag(x, y, z).

.decl says_hasTag(speaker: Principal, path: AccessPath, tag: Tag)
saysHasTag(x, y, z) :- says_hasTag(x, y, z).

.decl says_canSay_hasTag(
  speaker: Principal,
  delegatee: Principal,
  accessPath: AccessPath,
  tag: Tag)
saysCanSayHasTag(w, x, y, z) :- says_canSay_hasTag(w, x, y, z).

.decl says_canSay_removeTag(
    speaker: Principal, delegatee: Principal, path: AccessPath, tag: Tag)
saysCanSayRemoveTag(w, x, y, z) :- says_canSay_removeTag(w, x, y, z).

.decl says_removeTag(speaker: Principal, path: AccessPath, tag: Tag)
saysRemoveTag(x, y, z) :- says_removeTag(x, y, z).

.decl says_may(speaker: Principal, actor: Principal, usage: symbol, tag: Tag)
.decl says_will(speaker: Principal, usage: symbol, path: AccessPath)
saysMay(w, x, y, z) :- says_may(w, x, y, z).
saysWill(w, x, y) :- says_will(w, x, y).

// Manifest
%s
// Authorization Logic
%s
)";

};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_DATALOG_FACTS_H_
