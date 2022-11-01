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
#include "src/ir/attributes/string_attribute.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

TEST(StringAttributeKindTest, ConstantkAttributeKindIsString) {
  EXPECT_EQ(StringAttribute::kAttributeKind, AttributeBase::Kind::kString);
}
constexpr std::string_view kExampleStrings[] = {"good", "bad", "ugly",
                                                "3242e323"};

class StringAttributeTest : public testing::TestWithParam<std::string_view> {};

TEST_P(StringAttributeTest, KindAndToStringWorks) {
  std::string_view value = GetParam();
  auto string_attribute = StringAttribute::Create(value);
  EXPECT_EQ(string_attribute->kind(), StringAttribute::kAttributeKind);
  EXPECT_EQ(string_attribute->ToString(), absl::StrCat("\"", value, "\""));
}

INSTANTIATE_TEST_SUITE_P(StringAttributeTest, StringAttributeTest,
                         testing::ValuesIn(kExampleStrings));

class StringAttributeComparatorTest
    : public testing::TestWithParam<
          std::tuple<std::string_view, std::string_view>> {};

TEST_P(StringAttributeComparatorTest, EqualityWorksAsExpected) {
  const auto& [lhs, rhs] = GetParam();
  auto lhs_attribute = StringAttribute::Create(lhs);
  auto rhs_attribute = StringAttribute::Create(rhs);
  EXPECT_EQ(*lhs_attribute == *rhs_attribute, lhs == rhs);
}

INSTANTIATE_TEST_SUITE_P(StringAttributeComparatorTest,
                         StringAttributeComparatorTest,
                         testing::Combine(testing::ValuesIn(kExampleStrings),
                                          testing::ValuesIn(kExampleStrings)));

}  // namespace
}  // namespace raksha::ir
