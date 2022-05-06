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

#include "src/ir/datalog/attribute.h"

// #include "absl/strings/string_view.h"
#include "attribute.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::datalog {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

TEST(AttributeTest, NumberPayloadConstruction) {
  EXPECT_EQ(Attribute::Number(10).ToDatalogString(),
            "$NumberAttributePayload{10}");
  EXPECT_EQ(Attribute::Number(Number(42)).ToDatalogString(),
            "$NumberAttributePayload{42}");
}
TEST(AttributeTest, StringPayloadConstruction) {
  EXPECT_EQ(Attribute::String("hello world").ToDatalogString(),
            R"($StringAttributePayload{"hello world"})");
  EXPECT_EQ(Attribute::String(Symbol("42")).ToDatalogString(),
            R"($StringAttributePayload{"42"})");
}

TEST(AttributeTest, AttributeConstructionTest) {
  EXPECT_EQ(
      Attribute("name", Attribute::String("hello world")).ToDatalogString(),
      R"(["name", $StringAttributePayload{"hello world"}])");
  EXPECT_EQ(Attribute("answer_for_everything", Attribute::Number(42))
                .ToDatalogString(),
            R"(["answer_for_everything", $NumberAttributePayload{42}])");
}

}  // namespace raksha::ir::datalog
