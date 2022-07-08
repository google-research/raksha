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

#include "src/frontends/sql/driver.h"

#include "google/protobuf/text_format.h"
#include "src/backends/policy_engine/policy.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/ir/ir_printer.h"

namespace raksha::frontends::sql {

TEST(SimpleExpectedPassTest, SimpleExpectedPassTest) {
  // Make a simple expression arena containing a single string literal.
  std::string textproto =
      R"(id_expression_pairs: [ { id: 1 expression: { literal: { literal_str: "hello" } } } ])";
  ExpressionArena expression_arena;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(textproto,
                                                            &expression_arena));
  EXPECT_TRUE(verify(expression_arena, backends::policy_engine::Policy("")));
}

TEST(SimpleExpectedFailTest, SimpleExpectedFailTest) {
  std::string textproto =
      R"(
id_expression_pairs: [
  { id: 1 expression: { literal: { literal_str: "hello" } } },
  { id: 2 expression: { tag_transform: { transform_rule_name: "add_tag" transformed_node: 1 } } } ])";

  ExpressionArena expression_arena;
  EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(textproto,
                                                            &expression_arena));
  EXPECT_FALSE(
      verify(expression_arena,
             backends::policy_engine::Policy(
                 R"(["add_tag", $AddConfidentialityTag("tag"), nil])")));

  EXPECT_TRUE(verify(expression_arena, backends::policy_engine::Policy("")));
}

}  // namespace raksha::frontends::sql
