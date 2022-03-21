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
#include "src/ir/attributes/int_attribute.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

TEST(IntAttributeKindTest, ConstantkAttributeKindIsInt64) {
  EXPECT_EQ(Int64Attribute::kAttributeKind, AttributeBase::Kind::kInt64);
}

class IntAttributeTest
    : public testing::TestWithParam<std::pair<int64_t, absl::string_view>> {};

TEST_P(IntAttributeTest, KindAndToStringWorks) {
  const auto& [value, string_rep] = GetParam();
  auto int_attribute = Int64Attribute::Create(value);
  EXPECT_EQ(int_attribute->kind(), Int64Attribute::kAttributeKind);
  EXPECT_EQ(int_attribute->ToString(), string_rep);
}

INSTANTIATE_TEST_SUITE_P(IntAttributeTest, IntAttributeTest,
                         testing::Values(std::make_pair(10, "10"),
                                         std::make_pair(-30, "-30"),
                                         std::make_pair(0, "0")));

constexpr int kExampleInts[] = {10, -30, 0};

class IntAttributeComparatorTest
    : public testing::TestWithParam<std::tuple<int, int>> {};

TEST_P(IntAttributeComparatorTest, EqualityWorksAsExpected) {
  const auto& [lhs, rhs] = GetParam();
  auto lhs_attribute = Int64Attribute::Create(lhs);
  auto rhs_attribute = Int64Attribute::Create(rhs);
  EXPECT_EQ(*lhs_attribute == *rhs_attribute, lhs == rhs);
}

INSTANTIATE_TEST_SUITE_P(IntAttributeComparatorTest, IntAttributeComparatorTest,
                         testing::Combine(testing::ValuesIn(kExampleInts),
                                          testing::ValuesIn(kExampleInts)));

}  // namespace
}  // namespace raksha::ir
