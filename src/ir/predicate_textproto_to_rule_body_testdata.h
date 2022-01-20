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

#ifndef SRC_IR_PREDICATE_TEXTPROTO_TO_RULE_BODY_TESTDATA_H_H_
#define SRC_IR_PREDICATE_TEXTPROTO_TO_RULE_BODY_TESTDATA_H_H_

#include "absl/strings/substitute.h"

namespace raksha::ir {
// Note: StrFormat is careful not to allow dynamically constructed format
// strings. In the few cases that it does allow format strings to come from
// dynamic construction, it forces them to have a type indicating the format
// specifiers at static time through a template. Instead, we use Substitute,
// which is better suited for the purpose that we want. Therefore, access
// path substitution points are indicated with $0 instead of %s.
static constexpr std::tuple<absl::string_view, absl::string_view>
predicate_textproto_to_rule_body_format[] = {
    // Each of the tests is preceded by a snippet of what the predicate
    // might look like in an Arcs manifest file, with the AccessPath
    // represented by ap.

    // ap is tag1
    { R"(label: { semantic_tag: "tag1"})",
      R"($$CP_TagPresenceDynamicOwner("$0", "tag1"))" },

    // ap is tag1 and is tag2
    { R"(
and: {
  conjunct0: { label: { semantic_tag: "tag1"} }
  conjunct1: { label: { semantic_tag: "tag2"} } })",
  R"($$CP_And($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")))"},

  // ap is tag1 or is tag2
  { R"(
or: {
  disjunct0: { label: { semantic_tag: "tag1"} }
  disjunct1: { label: { semantic_tag: "tag2"} } })",
  R"($$CP_Or($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")))"},

  // ap is not tag1
  { R"(not: { predicate: { label: { semantic_tag: "tag1"} } })",
    R"($$CP_Not($$CP_TagPresenceDynamicOwner("$0", "tag1")))"},

  // ap is tag1 implies is tag2
  { R"(
implies: {
  antecedent: { label: { semantic_tag: "tag1"} }
  consequent: { label: { semantic_tag: "tag2"} } })",
    R"($$CP_Implies($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")))"
  },

  // ap is tag1 and (is tag2 or is tag3)
  { R"(
and: {
  conjunct0: { label: { semantic_tag: "tag1"} }
  conjunct1: {  or: {
    disjunct0: { label: { semantic_tag: "tag2"} }
    disjunct1: { label: { semantic_tag: "tag3"} } } } })",
  R"($$CP_And($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_Or($$CP_TagPresenceDynamicOwner("$0", "tag2"), $$CP_TagPresenceDynamicOwner("$0", "tag3"))))"},

  // ap (is tag1 or tag2) and is tag3
  { R"(
and: {
  conjunct0: {  or: {
    disjunct0: { label: { semantic_tag: "tag1"} }
    disjunct1: { label: { semantic_tag: "tag2"} } } }
  conjunct1: { label: { semantic_tag: "tag3"} } }
)",
  R"($$CP_And($$CP_Or($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")), $$CP_TagPresenceDynamicOwner("$0", "tag3")))"},
  // ap is tag1 or (is tag2 and is tag3)
  { R"(
or: {
  disjunct0: { label: { semantic_tag: "tag1"} }
  disjunct1: {  and: {
    conjunct0: { label: { semantic_tag: "tag2"} }
    conjunct1: { label: { semantic_tag: "tag3"} } } } })",
  R"($$CP_Or($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_And($$CP_TagPresenceDynamicOwner("$0", "tag2"), $$CP_TagPresenceDynamicOwner("$0", "tag3"))))"},

  // ap (is tag1 and tag2) or is tag3.
  { R"(
or: {
  disjunct0: {  and: {
    conjunct0: { label: { semantic_tag: "tag1"} }
    conjunct1: { label: { semantic_tag: "tag2"} } } }
  disjunct1: { label: { semantic_tag: "tag3"} } }
)",
  R"($$CP_Or($$CP_And($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")), $$CP_TagPresenceDynamicOwner("$0", "tag3")))"},

  // ap (is tag1 implies is tag2) implies (is tag3 implies is tag4)
  { R"(
implies: {
  antecedent: {
    implies: {
      antecedent: { label: { semantic_tag: "tag1"} }
      consequent: { label: { semantic_tag: "tag2"} }
    }
  }
  consequent: {
    implies: {
      antecedent: { label: { semantic_tag: "tag3"} }
      consequent: { label: { semantic_tag: "tag4"} }
    }
  } })",
     R"($$CP_Implies($$CP_Implies($$CP_TagPresenceDynamicOwner("$0", "tag1"), $$CP_TagPresenceDynamicOwner("$0", "tag2")), $$CP_Implies($$CP_TagPresenceDynamicOwner("$0", "tag3"), $$CP_TagPresenceDynamicOwner("$0", "tag4"))))"}
};
}  // namespace raksha::ir

#endif  // SRC_IR_PREDICATE_TEXTPROTO_TO_RULE_BODY_TESTDATA_H_H_
