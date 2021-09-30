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
    { R"(label: { semantic_tag: "tag1"})",
      R"(mayHaveTag("$0", "tag1"))" },
    { R"(
and: {
  conjunct0: { label: { semantic_tag: "tag1"} }
  conjunct1: { label: { semantic_tag: "tag2"} } })",
  R"(((mayHaveTag("$0", "tag1")), (mayHaveTag("$0", "tag2"))))"},
  { R"(
or: {
  disjunct0: { label: { semantic_tag: "tag1"} }
  disjunct1: { label: { semantic_tag: "tag2"} } })",
  R"(((mayHaveTag("$0", "tag1")); (mayHaveTag("$0", "tag2"))))"},
  { R"(not: { predicate: { label: { semantic_tag: "tag1"} } })",
    R"(!(mayHaveTag("$0", "tag1")))"},
  { R"(
implies: {
  antecedent: { label: { semantic_tag: "tag1"} }
  consequent: { label: { semantic_tag: "tag2"} } })",
     R"(!(mayHaveTag("$0", "tag1")); ((mayHaveTag("$0", "tag1")), (mayHaveTag("$0", "tag2"))))"
  },
  { R"(
and: {
  conjunct0: { label: { semantic_tag: "tag1"} }
  conjunct1: {  or: {
    disjunct0: { label: { semantic_tag: "tag2"} }
    disjunct1: { label: { semantic_tag: "tag3"} } } } })",
  R"(((mayHaveTag("$0", "tag1")), (((mayHaveTag("$0", "tag2")); (mayHaveTag("$0", "tag3"))))))"},
  { R"(
and: {
  conjunct0: {  or: {
    disjunct0: { label: { semantic_tag: "tag1"} }
    disjunct1: { label: { semantic_tag: "tag2"} } } }
  conjunct1: { label: { semantic_tag: "tag3"} } }
)",
  R"(((((mayHaveTag("$0", "tag1")); (mayHaveTag("$0", "tag2")))), (mayHaveTag("$0", "tag3"))))"},
  { R"(
or: {
  disjunct0: { label: { semantic_tag: "tag1"} }
  disjunct1: {  and: {
    conjunct0: { label: { semantic_tag: "tag2"} }
    conjunct1: { label: { semantic_tag: "tag3"} } } } })",
  R"(((mayHaveTag("$0", "tag1")); (((mayHaveTag("$0", "tag2")), (mayHaveTag("$0", "tag3"))))))"},
  { R"(
or: {
  disjunct0: {  and: {
    conjunct0: { label: { semantic_tag: "tag1"} }
    conjunct1: { label: { semantic_tag: "tag2"} } } }
  disjunct1: { label: { semantic_tag: "tag3"} } }
)",
  R"(((((mayHaveTag("$0", "tag1")), (mayHaveTag("$0", "tag2")))); (mayHaveTag("$0", "tag3"))))"},
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
     R"(!(!(mayHaveTag("$0", "tag1")); ((mayHaveTag("$0", "tag1")), (mayHaveTag("$0", "tag2")))); ((!(mayHaveTag("$0", "tag1")); ((mayHaveTag("$0", "tag1")), (mayHaveTag("$0", "tag2")))), (!(mayHaveTag("$0", "tag3")); ((mayHaveTag("$0", "tag3")), (mayHaveTag("$0", "tag4"))))))" }
};
}  // namespace raksha::ir

#endif  // SRC_IR_PREDICATE_TEXTPROTO_TO_RULE_BODY_TESTDATA_H_H_
