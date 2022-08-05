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
#include "src/ir/attributes/double_attribute.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

TEST(DoubleAttributeKindTest, ConstantkAttributeKindIsDouble) {
  EXPECT_EQ(DoubleAttribute::kAttributeKind, AttributeBase::Kind::kDouble);
}

class DoubleAttributeTest
    : public testing::TestWithParam<std::pair<double, absl::string_view>> {};

TEST_P(DoubleAttributeTest, KindAndToStringWorks) {
  const auto& [value, string_rep] = GetParam();
  auto double_attribute = DoubleAttribute::Create(value);
  EXPECT_EQ(double_attribute->kind(), DoubleAttribute::kAttributeKind);
  EXPECT_EQ(double_attribute->ToString(), string_rep);
}

INSTANTIATE_TEST_SUITE_P(DoubleAttributeTest, DoubleAttributeTest,
                         testing::Values(std::make_pair(10.5, "10.5"),
                                         std::make_pair(-30.01, "-30.01"),
                                         std::make_pair(0.999, "0.999")));

constexpr double kExampleDoubles[] = {10.5, -30.01, 0.999};

class DoubleAttributeComparatorTest
    : public testing::TestWithParam<std::tuple<double, double>> {};

TEST_P(DoubleAttributeComparatorTest, EqualityWorksAsExpected) {
  const auto& [lhs, rhs] = GetParam();
  auto lhs_attribute = DoubleAttribute::Create(lhs);
  auto rhs_attribute = DoubleAttribute::Create(rhs);
  EXPECT_EQ(*lhs_attribute == *rhs_attribute, lhs == rhs);
}

INSTANTIATE_TEST_SUITE_P(DoubleAttributeComparatorTest, DoubleAttributeComparatorTest,
                         testing::Combine(testing::ValuesIn(kExampleDoubles),
                                          testing::ValuesIn(kExampleDoubles)));

}  // namespace
}  // namespace raksha::ir

