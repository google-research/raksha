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
#include "src/ir/attributes/float_attribute.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

TEST(FloatAttributeKindTest, ConstantkAttributeKindIsFloat) {
  EXPECT_EQ(FloatAttribute::kAttributeKind, AttributeBase::Kind::kFloat);
}

class FloatAttributeTest
    : public testing::TestWithParam<std::pair<double, absl::string_view>> {};

TEST_P(FloatAttributeTest, KindAndToStringWorks) {
  const auto& [value, string_rep] = GetParam();
  auto float_attribute = FloatAttribute::Create(value);
  EXPECT_EQ(float_attribute->kind(), FloatAttribute::kAttributeKind);
  EXPECT_EQ(float_attribute->ToString(), string_rep);
}

INSTANTIATE_TEST_SUITE_P(FloatAttributeTest, FloatAttributeTest,
                         testing::Values(std::make_pair(10.5, "10.5l"),
                                         std::make_pair(-30.01, "-30.01l"),
                                         std::make_pair(0.999, "0.999l")));

constexpr double kExampleFloats[] = {10.5, -30.01, 0.999};

class FloatAttributeComparatorTest
    : public testing::TestWithParam<std::tuple<double, double>> {};

TEST_P(FloatAttributeComparatorTest, EqualityWorksAsExpected) {
  const auto& [lhs, rhs] = GetParam();
  auto lhs_attribute = FloatAttribute::Create(lhs);
  auto rhs_attribute = FloatAttribute::Create(rhs);
  EXPECT_EQ(*lhs_attribute == *rhs_attribute, lhs == rhs);
}

INSTANTIATE_TEST_SUITE_P(FloatAttributeComparatorTest, FloatAttributeComparatorTest,
                         testing::Combine(testing::ValuesIn(kExampleFloats),
                                          testing::ValuesIn(kExampleFloats)));

}  // namespace
}  // namespace raksha::ir

