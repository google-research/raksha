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
#include "src/ir/attribute.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {
namespace {

class IntAttributeTest
    : public testing::TestWithParam<std::pair<int64_t, absl::string_view>> {};

TEST_P(IntAttributeTest, KindAndToStringWorks) {
  const auto& [value, string_rep] = GetParam();
  auto int_attribute = Int64Attribute::Create(value);
  EXPECT_EQ(int_attribute->kind(), AttributeBase::Kind::kInt64);
  EXPECT_EQ(int_attribute->ToString(), string_rep);
}

INSTANTIATE_TEST_SUITE_P(IntAttributeTest, IntAttributeTest,
                         testing::Values(std::make_pair(10, "10"),
                                         std::make_pair(-30, "-30"),
                                         std::make_pair(0, "0")));

class StringAttributeTest : public testing::TestWithParam<absl::string_view> {};

TEST_P(StringAttributeTest, KindAndToStringWorks) {
  const auto& value = GetParam();
  auto string_attribute = StringAttribute::Create(value);
  EXPECT_EQ(string_attribute->kind(), AttributeBase::Kind::kString);
  EXPECT_EQ(string_attribute->ToString(), value);
}

INSTANTIATE_TEST_SUITE_P(StringAttributeTest, StringAttributeTest,
                         testing::Values("good", "bad", "ugly", "3242e323"));

}  // namespace
}  // namespace raksha::ir
